// i2s_player_engine.h
// I2S playback engine behind the i2s_player device.
//
// Plays 16 bit PCM WAV files from an SD card through I2S. Streaming runs in
// its own FreeRTOS task so that the iotempower main loop and the MQTT
// connection are never blocked. Only one instance can exist, because the
// engine owns I2S port 0 and the shared chunk buffers.

#ifndef _I2S_PLAYER_ENGINE_H_
#define _I2S_PLAYER_ENGINE_H_

#include <Arduino.h>

#ifdef ESP32

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#define I2S_PLAYER_MAX_FILE 128
#define I2S_PLAYER_STATUS_STRLEN 96

class I2S_Player_Engine {
    public:
        enum State : uint8_t {
            STATE_IDLE = 0,
            STATE_PLAYING,
            STATE_FINISHED,
            STATE_STOPPED,
            STATE_ERROR
        };

        struct Pins {
            uint8_t sd_cs;
            uint8_t sd_sck;
            uint8_t sd_mosi;
            uint8_t sd_miso;
            uint8_t i2s_bclk;
            uint8_t i2s_lrck;
            uint8_t i2s_data;
        };

        // Installs I2S, mounts the SD card and starts the player task.
        // Returns false if the I2S driver could not be installed. A missing
        // SD card does not fail here: playback then reports an error instead.
        bool begin(const Pins& pins, const char* audio_dir = "/audio");
        bool started() const { return _started; }

        // Base volume (0.0 - 1.0). Applied to the next chunk, so changes are
        // audible immediately, also during playback.
        void set_volume(float volume);
        float get_volume() const { return _volume; }

        // Loop mode for following playback requests. Changing it while a file
        // is running takes effect at that file's end.
        void set_loop(bool loop);
        bool get_loop() const { return _loop; }

        // Requests playback (last command wins). A file name starting with
        // '/' is used as given, otherwise it is resolved inside audio_dir.
        // volume < 0 keeps the base volume, otherwise the value is used for
        // this playback only. fade_in_ms/fade_out_ms ramp the gain at the
        // begin/end of the file (0 = only the short anti-click fade).
        void play(const char* file, float volume = -1.0f,
            uint16_t fade_in_ms = 0, uint16_t fade_out_ms = 0);
        // Stops the current playback with a fade and discards the DMA
        // buffer. fade_out_ms 0 uses the short default interrupt fade, so a
        // stop stays responsive.
        void stop(uint16_t fade_out_ms = 0);

        // Copies the status text if it changed since last_seq was updated.
        // Safe to call from any task.
        bool copy_status(char* destination, size_t destination_len,
            uint8_t& last_seq);

        State get_state() const { return (State)_state; }

    private:
        struct Command {
            enum Kind : uint8_t { PLAY, STOP } kind;
            char file[I2S_PLAYER_MAX_FILE];
            float volume;        // < 0: use the base volume
            uint16_t fade_in_ms; // 0: only the short anti-click fade
            uint16_t fade_out_ms;// 0: only the short anti-click fade
        };

        bool _started = false;
        Pins _pins = {0, 0, 0, 0, 0, 0, 0};
        char _audio_dir[24] = "/audio";
        QueueHandle_t _queue = NULL;
        TaskHandle_t _task = NULL;

        volatile float _volume = 0.5f;
        volatile bool _loop = false;

        volatile uint8_t _state = STATE_IDLE;
        char _status[I2S_PLAYER_STATUS_STRLEN] = "bereit";
        volatile uint8_t _status_seq = 0;
        portMUX_TYPE _status_mux = portMUX_INITIALIZER_UNLOCKED;

        bool i2s_setup();
        void play_file(const Command& request);
        void report_error(const char* file, const char* reason);
        void set_status(State state, const char* text);

        static void task_entry(void* argument);
        static void write_chunk(uint32_t frames);
};

#endif // ESP32

#endif // _I2S_PLAYER_ENGINE_H_
