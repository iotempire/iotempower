// i2s_player_engine.cpp
// I2S playback engine behind the i2s_player device.

#include "i2s_player_engine.h"

#ifdef ESP32

#include <SD.h>
#include <SPI.h>
#include <driver/i2s.h>
#include <stdio.h>
#include <string.h>

#include <toolbox.h>

#include "i2s_player_wav.h"

#define I2S_PLAYER_PORT ((i2s_port_t)0)
#define I2S_PLAYER_FRAMES_PER_CHUNK 1024
#define I2S_PLAYER_WRITE_TIMEOUT_MS 100
#define I2S_PLAYER_TASK_PRIO 3
#define I2S_PLAYER_TASK_STACK 4096
#define I2S_PLAYER_DEFAULT_RATE 16000
#define I2S_PLAYER_CLICK_FADE_MS 10
#define I2S_PLAYER_INTERRUPT_FADE_MS 100

// Output format. Stereo is the default, matching the design decision
// (LOUT/ROUT of the DAC each carry one channel). Define
// I2S_PLAYER_MONO_OUT to fold both channels into one mono stream
// (I2S_CHANNEL_FMT_ALL_LEFT), which is the variant verified in the audio2
// prototype. On ESP32-S2 the stereo slot layout was silent in that setup, so
// try mono first if this device stays quiet.
#ifndef I2S_PLAYER_MONO_OUT
#define I2S_PLAYER_MONO_OUT 0
#endif

#if I2S_PLAYER_MONO_OUT
#define I2S_PLAYER_OUTPUT_NAME "mono"
#else
#define I2S_PLAYER_OUTPUT_NAME "stereo"
#endif

// Raw input (1 or 2 channels, 16 bit) and staging buffer (interleaved L/R).
// File scope, because only one engine instance exists per device.
static int16_t i2s_player_in[I2S_PLAYER_FRAMES_PER_CHUNK * 2];
static int16_t i2s_player_out[I2S_PLAYER_FRAMES_PER_CHUNK * 2];

void I2S_Player_Engine::write_chunk(uint32_t frames) {
#if I2S_PLAYER_MONO_OUT
    // Downmix in place: the write index i never overtakes the read index
    // 2 * i, so a forward loop is safe.
    for (uint32_t i = 0; i < frames; i++) {
        i2s_player_out[i] = (int16_t)(((int32_t)i2s_player_out[2 * i] +
            (int32_t)i2s_player_out[2 * i + 1]) / 2);
    }
    const size_t total = (size_t)frames * 2;
#else
    const size_t total = (size_t)frames * 4;
#endif
    size_t offset = 0;
    while (offset < total) {
        size_t written = 0;
        esp_err_t rc = i2s_write(I2S_PLAYER_PORT, (uint8_t*)i2s_player_out + offset,
            total - offset, &written, pdMS_TO_TICKS(I2S_PLAYER_WRITE_TIMEOUT_MS));
        if (rc != ESP_OK || written == 0) {
            // Do not flood the log when the audio output is broken.
            static int failures = 0;
            if (failures < 3) {
                failures++;
                ulog(F("i2s_player: I2S write failed (rc=%d, %u of %u bytes)"),
                    (int)rc, (unsigned)written, (unsigned)(total - offset));
            }
            break;
        }
        offset += written;
    }
}

void I2S_Player_Engine::set_volume(float volume) {
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    _volume = volume;
}

void I2S_Player_Engine::set_loop(bool loop) {
    _loop = loop;
}

void I2S_Player_Engine::set_status(State state, const char* text) {
    portENTER_CRITICAL(&_status_mux);
    _state = state;
    if (text != NULL) {
        strncpy(_status, text, I2S_PLAYER_STATUS_STRLEN - 1);
        _status[I2S_PLAYER_STATUS_STRLEN - 1] = 0;
    }
    _status_seq++;
    portEXIT_CRITICAL(&_status_mux);
}

bool I2S_Player_Engine::copy_status(char* destination, size_t destination_len,
        uint8_t& last_seq) {
    bool changed = false;
    portENTER_CRITICAL(&_status_mux);
    if (_status_seq != last_seq) {
        last_seq = _status_seq;
        if (destination_len > 0) {
            strncpy(destination, _status, destination_len - 1);
            destination[destination_len - 1] = 0;
        }
        changed = true;
    }
    portEXIT_CRITICAL(&_status_mux);
    return changed;
}

void I2S_Player_Engine::report_error(const char* file, const char* reason) {
    char message[I2S_PLAYER_STATUS_STRLEN];
    snprintf(message, sizeof(message), "fehler: %s %s", file, reason);
    set_status(STATE_ERROR, message);
    ulog(F("i2s_player: cannot play >%s< (%s)"), file, reason);
}

void I2S_Player_Engine::play(const char* file) {
    if (!_queue || file == NULL || file[0] == 0) return;
    Command command;
    command.kind = Command::PLAY;
    strncpy(command.file, file, I2S_PLAYER_MAX_FILE - 1);
    command.file[I2S_PLAYER_MAX_FILE - 1] = 0;
    // Queue length is one, so the newest request always wins.
    xQueueOverwrite(_queue, &command);
}

void I2S_Player_Engine::stop() {
    if (!_queue) return;
    Command command;
    command.kind = Command::STOP;
    command.file[0] = 0;
    xQueueOverwrite(_queue, &command);
}

bool I2S_Player_Engine::i2s_setup() {
#if I2S_PLAYER_MONO_OUT
    const i2s_channel_fmt_t slot_format = I2S_CHANNEL_FMT_ALL_LEFT;
    const i2s_channel_t clock_channels = I2S_CHANNEL_MONO;
#else
    const i2s_channel_fmt_t slot_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    const i2s_channel_t clock_channels = I2S_CHANNEL_STEREO;
#endif
    i2s_config_t config = {};
    config.mode = (i2s_mode_t)(I2S_MODE_TX | I2S_MODE_MASTER);
    config.sample_rate = I2S_PLAYER_DEFAULT_RATE;
    config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    config.channel_format = slot_format;
    config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
    config.dma_buf_count = 8;
    config.dma_buf_len = 256;
    config.use_apll = true;
    config.tx_desc_auto_clear = true;
    config.fixed_mclk = 0;

    if (i2s_driver_install(I2S_PLAYER_PORT, &config, 0, NULL) != ESP_OK) {
        ulog(F("i2s_player: installing the I2S driver failed."));
        return false;
    }
    i2s_pin_config_t pin_config = {};
    pin_config.bck_io_num = _pins.i2s_bclk;
    pin_config.ws_io_num = _pins.i2s_lrck;
    pin_config.data_out_num = _pins.i2s_data;
    pin_config.data_in_num = I2S_PIN_NO_CHANGE;
    if (i2s_set_pin(I2S_PLAYER_PORT, &pin_config) != ESP_OK) {
        ulog(F("i2s_player: assigning the I2S pins failed."));
        return false;
    }
    esp_err_t rc = i2s_set_clk(I2S_PLAYER_PORT, I2S_PLAYER_DEFAULT_RATE,
        I2S_BITS_PER_SAMPLE_16BIT, clock_channels);
    if (rc != ESP_OK) {
        ulog(F("i2s_player: setting the initial I2S clock failed (rc=%d)."), (int)rc);
    }
    i2s_start(I2S_PLAYER_PORT);
    ulog(F("i2s_player: I2S ready (%s output, BCLK %u, LRCK %u, DATA %u)."),
        I2S_PLAYER_OUTPUT_NAME, (unsigned)_pins.i2s_bclk,
        (unsigned)_pins.i2s_lrck, (unsigned)_pins.i2s_data);
    return true;
}

bool I2S_Player_Engine::begin(const Pins& pins, const char* audio_dir) {
    if (_started) return true;
    _pins = pins;
    if (audio_dir != NULL) {
        strncpy(_audio_dir, audio_dir, sizeof(_audio_dir) - 1);
        _audio_dir[sizeof(_audio_dir) - 1] = 0;
    }

    _queue = xQueueCreate(1, sizeof(Command));
    if (!_queue) {
        ulog(F("i2s_player: cannot create the command queue."));
        return false;
    }
    if (!i2s_setup()) return false;

    SPI.begin(_pins.sd_sck, _pins.sd_miso, _pins.sd_mosi, _pins.sd_cs);
    if (!SD.begin(_pins.sd_cs)) {
        // Playback will fail with a status message; the device itself stays
        // reachable so that the problem is visible over MQTT.
        ulog(F("i2s_player: SD card not available (CS %u)."), (unsigned)_pins.sd_cs);
    } else {
        ulog(F("i2s_player: SD card ready (%s)."), _audio_dir);
    }

    BaseType_t created = xTaskCreate(task_entry, "i2s_player", I2S_PLAYER_TASK_STACK,
        this, I2S_PLAYER_TASK_PRIO, &_task);
    if (created != pdPASS) {
        ulog(F("i2s_player: cannot create the player task."));
        return false;
    }

    set_status(STATE_IDLE, "bereit");
    _started = true;
    return true;
}

void I2S_Player_Engine::play_file(const char* file) {
    char path[I2S_PLAYER_MAX_FILE + 32];
    if (file[0] == '/') {
        snprintf(path, sizeof(path), "%s", file);
    } else {
        snprintf(path, sizeof(path), "%s/%s", _audio_dir, file);
    }

    File f = SD.open(path);
    if (!f) {
        report_error(file, "fehlt");
        return;
    }

    I2S_Wav_Info info;
    if (!i2s_wav_read_info(f, info) || info.bits != 16
            || info.channels < 1 || info.channels > 2) {
        f.close();
        report_error(file, "format");
        return;
    }
    const uint32_t frame_bytes = (uint32_t)info.channels * 2;
    if (info.data_size < frame_bytes) {
        f.close();
        report_error(file, "leer");
        return;
    }
    if (!f.seek(info.data_offset)) {
        f.close();
        report_error(file, "format");
        return;
    }

    i2s_channel_t clock_channels = I2S_CHANNEL_STEREO;
#if I2S_PLAYER_MONO_OUT
    clock_channels = I2S_CHANNEL_MONO;
#endif
    if (i2s_set_clk(I2S_PLAYER_PORT, info.sample_rate, I2S_BITS_PER_SAMPLE_16BIT,
            clock_channels) != ESP_OK) {
        ulog(F("i2s_player: cannot set the I2S clock to %u Hz."),
            (unsigned)info.sample_rate);
    }

    uint32_t click_fade = (info.sample_rate / 1000) * I2S_PLAYER_CLICK_FADE_MS;
    if (click_fade == 0) click_fade = 1;
    uint32_t interrupt_fade =
        (uint32_t)(((uint64_t)I2S_PLAYER_INTERRUPT_FADE_MS * info.sample_rate) / 1000);
    if (interrupt_fade == 0) interrupt_fade = 1;

    {
        char message[I2S_PLAYER_STATUS_STRLEN];
        snprintf(message, sizeof(message), "spielt %s", file);
        set_status(STATE_PLAYING, message);
    }

    bool interrupted = false;
    uint32_t fade_position = 0;
    uint32_t played_frames = 0;

    for (;;) {
        // A new request is handled between two chunks: STOP ends the file
        // after a short fade, a new PLAY keeps the newest command queued.
        Command pending;
        if (xQueuePeek(_queue, &pending, 0) == pdTRUE) {
            if (!interrupted || pending.kind == Command::PLAY) {
                if (pending.kind == Command::STOP) {
                    xQueueReceive(_queue, &pending, 0);
                }
                interrupted = true;
                fade_position = 0;
            }
        }

        int read = f.read((uint8_t*)i2s_player_in,
            (size_t)(I2S_PLAYER_FRAMES_PER_CHUNK * frame_bytes));
        if (read <= 0) {
            if (_loop && !interrupted && f.seek(info.data_offset)) continue;
            break;
        }
        int frames = read / (int)frame_bytes;
        if (frames <= 0) break;

        const float base_gain = _volume;
        for (int i = 0; i < frames; i++) {
            float gain = base_gain;
            // Short fade in at the start of the file, avoids a click.
            if (played_frames + (uint32_t)i < click_fade) {
                gain *= (float)(played_frames + (uint32_t)i) / (float)click_fade;
            }
            if (interrupted) {
                uint32_t pos = fade_position + (uint32_t)i;
                if (pos >= interrupt_fade) {
                    gain = 0.0f;
                } else {
                    gain *= 1.0f - (float)pos / (float)interrupt_fade;
                }
            }

            // Mono goes to both channels, stereo stays untouched.
            int32_t left;
            int32_t right;
            if (info.channels == 1) {
                left = right = i2s_player_in[i];
            } else {
                left = i2s_player_in[2 * i];
                right = i2s_player_in[2 * i + 1];
            }
            i2s_player_out[2 * i] = (int16_t)((float)left * gain);
            i2s_player_out[2 * i + 1] = (int16_t)((float)right * gain);
        }

        write_chunk((uint32_t)frames);
        played_frames += (uint32_t)frames;

        if (interrupted) {
            fade_position += (uint32_t)frames;
            if (fade_position >= interrupt_fade) break;
        }
    }

    f.close();

    if (interrupted) {
        i2s_zero_dma_buffer(I2S_PLAYER_PORT);
        // Only report the stop if no new PLAY is waiting; a waiting PLAY
        // reports its own status as soon as the task picks it up.
        Command pending;
        if (xQueuePeek(_queue, &pending, 0) != pdTRUE) {
            set_status(STATE_STOPPED, "gestoppt");
        }
    } else {
        set_status(STATE_FINISHED, "fertig");
    }
}

void I2S_Player_Engine::task_entry(void* argument) {
    I2S_Player_Engine* self = (I2S_Player_Engine*)argument;
    for (;;) {
        Command command;
        if (xQueueReceive(self->_queue, &command, portMAX_DELAY) != pdTRUE) continue;
        if (command.kind == Command::PLAY) {
            self->play_file(command.file);
        } else if (command.kind == Command::STOP) {
            // STOP without playback: just make sure the output is silent.
            i2s_zero_dma_buffer(I2S_PLAYER_PORT);
            self->set_status(STATE_STOPPED, "gestoppt");
        }
    }
}

#endif // ESP32
