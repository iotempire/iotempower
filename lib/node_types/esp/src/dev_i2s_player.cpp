// dev_i2s_player.cpp

#include "dev_i2s_player.h"

#ifdef ESP32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <toolbox.h>

#include "audio_source.h"
#include "audio_status.h"

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
        : Device(name, 200000) { // measure() only forwards status events
    _out.set_pins(i2s_bclk, i2s_lrck, i2s_data);

    // Payloads are not lowercased: the source decides what they mean, and file
    // names on a card are case sensitive.
    with_ignore_case(false);

    // Volume is retained, so a dashboard or Node-RED knows the current level
    // right after a restart.
    add_subdevice(new Subdevice(F("volume")))->with_retained(true);
    // Status is the WLAN visible feedback: idle, playing <file> or
    // error: <reason>. It is filled in measure() from the status queue.
    add_subdevice(new Subdevice(F("status")))->with_retained(true);
    // One command topic for the whole device: the first word is the verb, the
    // rest is its argument. Keeps commands short and readable:
    //   player/set  play 440
    //   player/set  stop
    //   player/set  volume 30
    add_subdevice(new Subdevice(F("set"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            command(payload);
            return true;
        }
    );

    value(I2S_PLAYER_SUB_STATUS).from(F("idle"));
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

// Case insensitive comparison of two zero terminated strings.
static bool i2s_player_text_is(const char* text, const char* expected) {
    while (*text != 0 && *expected != 0) {
        char c = *text;
        if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        if (c != *expected) return false;
        text++;
        expected++;
    }
    return *text == 0 && *expected == 0;
}

// Splits a trailing routing suffix (":left", ":right", ":both", short ":l",
// ":r", ":b", case insensitive) from the file name. Returns false for an
// unknown suffix, so a typo does not silently become part of a file name.
static bool i2s_player_split_routing(const char* argument, char* name,
        size_t size, I2S_Player_Routing& routing) {
    routing = I2S_PLAYER_ROUTING_BOTH;
    const char* colon = strrchr(argument, ':');
    if (colon == NULL) {
        strncpy(name, argument, size - 1);
        name[size - 1] = 0;
        return true;
    }

    const char* suffix = colon + 1;
    if (i2s_player_text_is(suffix, "left") || i2s_player_text_is(suffix, "l")) {
        routing = I2S_PLAYER_ROUTING_LEFT;
    } else if (i2s_player_text_is(suffix, "right")
            || i2s_player_text_is(suffix, "r")) {
        routing = I2S_PLAYER_ROUTING_RIGHT;
    } else if (i2s_player_text_is(suffix, "both")
            || i2s_player_text_is(suffix, "b")) {
        routing = I2S_PLAYER_ROUTING_BOTH;
    } else {
        return false;
    }

    size_t length = (size_t)(colon - argument);
    if (length >= size) length = size - 1;
    memcpy(name, argument, length);
    name[length] = 0;
    return true;
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
        char name[I2S_PLAYER_MAX_PAYLOAD];
        I2S_Player_Routing routing = I2S_PLAYER_ROUTING_BOTH;
        if (!i2s_player_split_routing(argument, name, sizeof(name), routing)) {
            ulog(F("i2s_player: bad routing >%s< (use :left, :right or :both)."),
                argument);
            audio_status_event("error: bad routing");
            return;
        }
        request(Command::PLAY, name, routing);
    } else if (i2s_player_verb_is(start, verb_length, "stop")) {
        request(Command::STOP, "");
    } else if (i2s_player_verb_is(start, verb_length, "volume")) {
        if (*argument == 0) {
            ulog(F("i2s_player: volume needs a value (0..100)."));
            return;
        }
        char* end = NULL;
        long volume = strtol(argument, &end, 10);
        if (end == argument || *end == '.' || *end == ',') {
            // Catch the old 0.0 - 1.0 values instead of silently muting.
            ulog(F("i2s_player: volume needs an integer 0..100, >%s< ignored."),
                argument);
            return;
        }
        if (volume < 0) volume = 0;
        if (volume > 100) volume = 100;
        _volume = (uint8_t)volume;
        value(I2S_PLAYER_SUB_VOLUME).from(volume);
    } else {
        ulog(F("i2s_player: unknown command >%s<."), text.as_cstr());
    }
}

void I2S_Player::request(Command::Kind kind, const char* payload,
        I2S_Player_Routing routing) {
    if (_queue == NULL) return;
    Command command;
    command.kind = kind;
    command.routing = routing;
    if (payload == NULL) payload = "";
    strncpy(command.payload, payload, I2S_PLAYER_MAX_PAYLOAD - 1);
    command.payload[I2S_PLAYER_MAX_PAYLOAD - 1] = 0;
    // Queue length is one, so the newest request always wins.
    xQueueOverwrite(_queue, &command);
}

void I2S_Player::start() {
    audio_status_start();
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
    value(I2S_PLAYER_SUB_VOLUME).from((long)_volume);
    value(I2S_PLAYER_SUB_STATUS).from(F("idle"));
    _started = true;
}

void I2S_Player::play_stream(const char* payload,
        I2S_Player_Routing routing) {
    audio_status_clear_error();

    Audio_Source* source = audio_sources().first();
    if (source == NULL) {
        ulog(F("i2s_player: no audio source registered."));
        audio_status_error("error: no source");
        return;
    }
    if (!source->open(payload)) {
        // The source reports its own reason; this is only a fallback.
        if (!audio_status_has_error()) audio_status_error("error: open");
        return;
    }

    const uint32_t rate = source->sample_rate();
    const uint8_t channels = source->channels();
    if (channels < 1 || channels > 2 || !_out.set_rate(rate)) {
        audio_status_error("error: no i2s");
        source->close();
        return;
    }

    char playing[AUDIO_STATUS_MAX_LENGTH];
    if (routing == I2S_PLAYER_ROUTING_BOTH) {
        snprintf(playing, sizeof(playing), "playing %s", payload);
    } else {
        snprintf(playing, sizeof(playing), "playing %s:%s", payload,
            routing == I2S_PLAYER_ROUTING_LEFT ? "left" : "right");
    }
    audio_status_event(playing);

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
        const float base_gain = (float)_volume / 100.0f;
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

            // Mono goes to both channels, stereo keeps its own sides. The
            // routing mutes the side that was not asked for.
            int32_t left;
            int32_t right;
            if (channels == 1) {
                left = right = i2s_player_in[i];
            } else {
                left = i2s_player_in[2 * i];
                right = i2s_player_in[2 * i + 1];
            }
            if (routing == I2S_PLAYER_ROUTING_LEFT) {
                right = 0;
            } else if (routing == I2S_PLAYER_ROUTING_RIGHT) {
                left = 0;
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
    // After a read error the status must keep the error instead of "idle".
    if (!audio_status_has_error()) audio_status_event("idle");
}

bool I2S_Player::measure() {
    // measure() runs in the main loop, so the status value is only touched
    // here and never from the player task.
    char message[AUDIO_STATUS_MAX_LENGTH];
    if (audio_status_take(message, sizeof(message))) {
        value(I2S_PLAYER_SUB_STATUS).from(message);
    }
    // Always report success: a false return would make the framework replace
    // every measured value with the last published one, dropping changes that
    // arrived between two polls (for example a new volume).
    return true;
}

void I2S_Player::task_entry(void* argument) {
    I2S_Player* self = (I2S_Player*)argument;
    for (;;) {
        Command command;
        if (xQueueReceive(self->_queue, &command, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (command.kind == Command::PLAY) {
            self->play_stream(command.payload, command.routing);
        } else {
            // STOP while nothing is playing: just make sure it stays silent.
            self->_out.silence();
            audio_status_event("idle");
        }
    }
}

#endif // ESP32
