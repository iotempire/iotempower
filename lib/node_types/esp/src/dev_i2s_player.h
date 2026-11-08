// dev_i2s_player.h
// I2S audio player device: plays 16 bit PCM WAV files from an SD card.
//
// Topics (with <node> being the MQTT topic prefix of the node):
//   <node>/player/play/set    file name, e.g. "begruessung.wav" or
//                             "/audio/begruessung.wav"
//   <node>/player/stop/set    any payload stops the current playback
//   <node>/player/volume      base volume 0.0 - 1.0 (state)
//   <node>/player/volume/set  base volume 0.0 - 1.0 (command)
//   <node>/player/status      "bereit", "spielt <file>", "fertig",
//                             "gestoppt" or "fehler: <file> <reason>"
//   <node>/player/loop        "true" / "false" (state)
//   <node>/player/loop/set    "on"/"off" or "true"/"false" (command)
//
// Use it in setup.cpp like this (pin order: SD CS, SCK, MOSI, MISO,
// I2S BCLK, LRCK, DATA):
//   i2s_player(player, 2, 4, 6, 8, 40, 36, 38);

#ifndef _I2S_PLAYER_H_
#define _I2S_PLAYER_H_

#include <device.h>

// Subdevice indices in the order they are registered below.
#define I2S_PLAYER_SUB_PLAY 0
#define I2S_PLAYER_SUB_STOP 1
#define I2S_PLAYER_SUB_VOLUME 2
#define I2S_PLAYER_SUB_VOLUME_SET 3
#define I2S_PLAYER_SUB_STATUS 4
#define I2S_PLAYER_SUB_LOOP 5
#define I2S_PLAYER_SUB_LOOP_SET 6

#ifdef ESP32

#include "i2s_player_engine.h"

class I2S_Player : public Device {
    private:
        I2S_Player_Engine _engine;
        I2S_Player_Engine::Pins _pins;
        float _volume = 0.5f;
        bool _loop = false;
        uint8_t _status_seq = 0;

        void set_volume(float volume);
        void set_loop(bool loop);

    public:
        I2S_Player(const char* name,
            uint8_t sd_cs, uint8_t sd_sck, uint8_t sd_mosi, uint8_t sd_miso,
            uint8_t i2s_bclk, uint8_t i2s_lrck, uint8_t i2s_data);
        void start();
        bool measure();
};

#else

// I2S playback exists only on ESP32. On other platforms the device type stays
// compilable and simply remains inactive.
class I2S_Player : public Device {
    public:
        I2S_Player(const char* name, uint8_t, uint8_t, uint8_t, uint8_t,
            uint8_t, uint8_t, uint8_t) : Device(name, 200000) {}
        void start() {
            ulog(F("i2s_player: only supported on ESP32 devices."));
        }
};

#endif // ESP32

#endif // _I2S_PLAYER_H_
