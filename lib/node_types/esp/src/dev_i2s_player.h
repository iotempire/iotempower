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
//                                 play a.wav:left
//                                              only the left channel
//                                              (:right, :both, also :l/:r/:b)
//                                 stop         stops playback
//                                 volume 30    sets the volume 0 - 100
//   <node>/player/volume        volume 0 - 100 (state, retained, percent)
//   <node>/player/status        idle | playing <file> | error: <reason>
//                               (state, retained)
//
// The status topic is what is visible over WLAN: errors that otherwise only
// show up in the serial log are published as "error: ..." here. The framework
// republishes every value every transmission_delta (default 5 s).
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
#define I2S_PLAYER_SUB_STATUS 1

// Which output channels a played file is routed to. The suffix is part of the
// play payload: "play a.wav:left".
enum I2S_Player_Routing : uint8_t {
    I2S_PLAYER_ROUTING_BOTH = 0,
    I2S_PLAYER_ROUTING_LEFT,
    I2S_PLAYER_ROUTING_RIGHT
};

class I2S_Player : public Device {
    public:
        I2S_Player(const char* name, uint8_t i2s_bclk, uint8_t i2s_lrck,
            uint8_t i2s_data);
        void start();
        bool measure();

    private:
        struct Command {
            enum Kind : uint8_t { PLAY, STOP } kind;
            I2S_Player_Routing routing;
            char payload[I2S_PLAYER_MAX_PAYLOAD];
        };

        I2S_Out _out;
        QueueHandle_t _queue = NULL;
        TaskHandle_t _task = NULL;
        volatile uint8_t _volume = 50; // percent, 0 - 100

        void request(Command::Kind kind, const char* payload,
            I2S_Player_Routing routing = I2S_PLAYER_ROUTING_BOTH);
        void command(const Ustring& payload);
        void play_stream(const char* payload, I2S_Player_Routing routing);
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
