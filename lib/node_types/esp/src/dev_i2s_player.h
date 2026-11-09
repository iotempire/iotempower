// dev_i2s_player.h
// Pure I2S output player for 16 bit PCM audio.
//
// The player owns the I2S output and the playback task. Where the samples
// come from is decided by an Audio_Source (see audio_source.h): the tone
// device today, an SD card or the internal flash later. The player itself
// knows nothing about files or storage media.
//
// Topics (with <node> being the MQTT topic prefix of the node):
//
//   <node>/player/set           command topic: "<verb> [argument]"
//                                 play 440     starts the source with the
//                                              argument (tone: frequency)
//                                 stop         stops playback
//                                 volume 0.3   sets the volume 0.0 - 1.0
//   <node>/player/volume        volume 0.0 - 1.0 (state, retained)
//
// There is no status topic: the framework republishes every value every
// transmission_delta (default 5 s), and errors are reported in the log.
//
// Use it in setup.cpp like this (I2S pins: BCLK, LRCK, DATA):
//   i2s_tone(quelle, 440);
//   i2s_player(player, 40, 36, 38);

#ifndef _I2S_PLAYER_H_
#define _I2S_PLAYER_H_

#include <device.h>

#ifdef ESP32

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include "i2s_out.h"

#define I2S_PLAYER_MAX_PAYLOAD 128

// Subdevice indices in the order they are registered below.
#define I2S_PLAYER_SUB_VOLUME 0

class I2S_Player : public Device {
    public:
        I2S_Player(const char* name, uint8_t i2s_bclk, uint8_t i2s_lrck,
            uint8_t i2s_data);
        void start();

    private:
        struct Command {
            enum Kind : uint8_t { PLAY, STOP } kind;
            char payload[I2S_PLAYER_MAX_PAYLOAD];
        };

        I2S_Out _out;
        QueueHandle_t _queue = NULL;
        TaskHandle_t _task = NULL;
        volatile float _volume = 0.5f;

        void request(Command::Kind kind, const char* payload);
        void command(const Ustring& payload);
        void play_stream(const char* payload);
        static void task_entry(void* argument);
};

#else

// I2S playback exists only on ESP32. On other platforms the device type stays
// compilable and simply remains inactive.
class I2S_Player : public Device {
    public:
        I2S_Player(const char* name, uint8_t, uint8_t, uint8_t)
            : Device(name, 1000000) {}
        void start() {
            ulog(F("i2s_player: only supported on ESP32 devices."));
        }
};

#endif // ESP32

#endif // _I2S_PLAYER_H_
