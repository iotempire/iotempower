// dev_i2s_player.cpp

#include "dev_i2s_player.h"

#ifdef ESP32

#include <stdlib.h>
#include <string.h>

#include <toolbox.h>

#include "audio_source.h"

#define I2S_PLAYER_FRAMES_PER_CHUNK 1024
#define I2S_PLAYER_CLICK_FADE_MS 10
#define I2S_PLAYER_INTERRUPT_FADE_MS 100
#define I2S_PLAYER_WRITE_TIMEOUT_MS 100
#define I2S_PLAYER_TASK_PRIO 3
#define I2S_PLAYER_TASK_STACK 4096

// One player instance per node: the I2S port is a single hardware resource.
// Both buffers are interleaved stereo, the input may carry 1 or 2 channels.
static int16_t i2s_player_in[I2S_PLAYER_FRAMES_PER_CHUNK * 2];
static int16_t i2s_player_out[I2S_PLAYER_FRAMES_PER_CHUNK * 2];

// Number of frames that cover a fade of ms milliseconds at the given rate.
// Never returns 0, so the gain ramp cannot divide by zero.
static uint32_t i2s_player_frames_for_ms(uint16_t ms, uint32_t rate) {
    uint32_t frames = (uint32_t)(((uint64_t)ms * rate) / 1000);
    return frames == 0 ? 1 : frames;
}

I2S_Player::I2S_Player(const char* name, uint8_t i2s_bclk, uint8_t i2s_lrck,
        uint8_t i2s_data)
        : Device(name, 1000000) { // nothing is polled, commands drive the device
    _out.set_pins(i2s_bclk, i2s_lrck, i2s_data);

    // Payloads are not lowercased: the source decides what they mean, and file
    // names on a card are case sensitive.
    with_ignore_case(false);

    // Volume is retained, so a dashboard or Node-RED knows the current level
    // right after a restart.
    add_subdevice(new Subdevice(F("volume")))->with_retained(true);
    // One command topic for the whole device: the first word is the verb, the
    // rest is its argument. Keeps commands short and readable:
    //   player/set  play 440
    //   player/set  stop
    //   player/set  volume 0.3
    add_subdevice(new Subdevice(F("set"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            command(payload);
            return true;
        }
    );
}

// Compares a verb that is not zero terminated with a lowercase literal,
// ignoring case.
static bool i2s_player_verb_is(const char* verb, size_t length,
        const char* expected) {
    size_t i = 0;
    for (; i < length; i++) {
        char c = verb[i];
        if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        if (expected[i] == 0 || c != expected[i]) return false;
    }
    return expected[i] == 0;
}

void I2S_Player::command(const Ustring& payload) {
    Ustring text(payload);
    text.strip();
    if (text.empty()) {
        ulog(F("i2s_player: empty command ignored."));
        return;
    }

    const char* start = text.as_cstr();
    size_t verb_length = 0;
    while (start[verb_length] != 0 && start[verb_length] != ' '
            && start[verb_length] != '\t') {
        verb_length++;
    }
    const char* argument = start + verb_length;
    while (*argument == ' ' || *argument == '\t') argument++;

    if (i2s_player_verb_is(start, verb_length, "play")) {
        request(Command::PLAY, argument);
    } else if (i2s_player_verb_is(start, verb_length, "stop")) {
        request(Command::STOP, "");
    } else if (i2s_player_verb_is(start, verb_length, "volume")) {
        if (*argument == 0) {
            ulog(F("i2s_player: volume needs a value (0..1)."));
            return;
        }
        float volume = strtof(argument, NULL);
        if (volume < 0.0f) volume = 0.0f;
        if (volume > 1.0f) volume = 1.0f;
        _volume = volume;
        value(I2S_PLAYER_SUB_VOLUME).from(volume);
    } else {
        ulog(F("i2s_player: unknown command >%s<."), text.as_cstr());
    }
}

void I2S_Player::request(Command::Kind kind, const char* payload) {
    if (_queue == NULL) return;
    Command command;
    command.kind = kind;
    if (payload == NULL) payload = "";
    strncpy(command.payload, payload, I2S_PLAYER_MAX_PAYLOAD - 1);
    command.payload[I2S_PLAYER_MAX_PAYLOAD - 1] = 0;
    // Queue length is one, so the newest request always wins.
    xQueueOverwrite(_queue, &command);
}

void I2S_Player::start() {
    _queue = xQueueCreate(1, sizeof(Command));
    if (_queue == NULL) {
        ulog(F("i2s_player: cannot create the command queue."));
        return;
    }
    if (xTaskCreate(task_entry, "i2s_player", I2S_PLAYER_TASK_STACK, this,
            I2S_PLAYER_TASK_PRIO, &_task) != pdPASS) {
        ulog(F("i2s_player: cannot create the player task."));
        return;
    }
    value(I2S_PLAYER_SUB_VOLUME).from(_volume);
    _started = true;
}

void I2S_Player::play_stream(const char* payload) {
    Audio_Source* source = audio_sources().first();
    if (source == NULL) {
        ulog(F("i2s_player: no audio source registered."));
        return;
    }
    if (!source->open(payload)) return;

    const uint32_t rate = source->sample_rate();
    const uint8_t channels = source->channels();
    if (channels < 1 || channels > 2 || !_out.set_rate(rate)) {
        source->close();
        return;
    }

    // Short ramp at the start of every stream, so it does not click.
    const uint32_t click_fade = i2s_player_frames_for_ms(
        I2S_PLAYER_CLICK_FADE_MS, rate);
    // Ramp used when the playback is interrupted (stop or a new play).
    const uint32_t interrupt_fade = i2s_player_frames_for_ms(
        I2S_PLAYER_INTERRUPT_FADE_MS, rate);

    uint32_t played = 0;
    uint32_t fade_position = 0;
    bool interrupted = false;

    for (;;) {
        // A new request is handled between two chunks: STOP ends the stream
        // after a short fade, a new PLAY keeps the newest command queued.
        Command pending;
        if (xQueuePeek(_queue, &pending, 0) == pdTRUE) {
            // A stop is always consumed, a play stays queued for the task
            // loop (the newest command wins).
            if (pending.kind == Command::STOP) {
                xQueueReceive(_queue, &pending, 0);
            }
            // Start the interrupt fade exactly once: resetting it on every
            // chunk would restart the ramp and never finish.
            if (!interrupted) {
                interrupted = true;
                fade_position = 0;
            }
        }

        int got = source->read(i2s_player_in, I2S_PLAYER_FRAMES_PER_CHUNK);
        if (got == 0) break;
        if (got < 0) {
            // A stream source has no data right now: keep the playback alive.
            vTaskDelay(pdMS_TO_TICKS(5));
            continue;
        }

        // Read the live volume once per chunk, so changes during playback are
        // audible immediately.
        const float base_gain = _volume;
        for (int i = 0; i < got; i++) {
            float gain = base_gain;
            const uint32_t position = played + (uint32_t)i;
            if (position < click_fade) {
                gain *= (float)position / (float)click_fade;
            }
            if (interrupted) {
                const uint32_t pos = fade_position + (uint32_t)i;
                if (pos >= interrupt_fade) {
                    gain = 0.0f;
                } else {
                    gain *= 1.0f - (float)pos / (float)interrupt_fade;
                }
            }

            // Mono goes to both channels, stereo stays untouched.
            int32_t left;
            int32_t right;
            if (channels == 1) {
                left = right = i2s_player_in[i];
            } else {
                left = i2s_player_in[2 * i];
                right = i2s_player_in[2 * i + 1];
            }
            i2s_player_out[2 * i] = (int16_t)((float)left * gain);
            i2s_player_out[2 * i + 1] = (int16_t)((float)right * gain);
        }

        _out.write(i2s_player_out, (size_t)got, I2S_PLAYER_WRITE_TIMEOUT_MS);
        played += (uint32_t)got;

        if (interrupted) {
            fade_position += (uint32_t)got;
            if (fade_position >= interrupt_fade) break;
        }
    }

    source->close();
    if (interrupted) _out.silence();
}

void I2S_Player::task_entry(void* argument) {
    I2S_Player* self = (I2S_Player*)argument;
    for (;;) {
        Command command;
        if (xQueueReceive(self->_queue, &command, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (command.kind == Command::PLAY) {
            self->play_stream(command.payload);
        } else {
            // STOP while nothing is playing: just make sure it stays silent.
            self->_out.silence();
        }
    }
}

#endif // ESP32
