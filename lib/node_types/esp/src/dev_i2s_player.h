// dev_i2s_player.h
// I2S audio player device: plays 16 bit PCM WAV files from an SD card.
//
// Topics (with <node> being the MQTT topic prefix of the node). Every payload
// is a single value: a file name, a number or on/off.
//
//   <node>/player/play/set      file name, e.g. "begruessung.wav" (resolved
//                               inside /audio) or "/audio/begruessung.wav"
//   <node>/player/stop/set      stops the playback; an optional payload is
//                               the fade out time in ms (empty = the short
//                               interrupt fade, so a stop stays responsive)
//   <node>/player/volume        base volume 0.0 - 1.0 (state, retained)
//   <node>/player/volume/set    base volume 0.0 - 1.0 (command, live)
//   <node>/player/fade_in       fade in time in ms (state, retained)
//   <node>/player/fade_in/set   fade in time in ms (command)
//   <node>/player/fade_out      fade out time in ms (state, retained)
//   <node>/player/fade_out/set  fade out time in ms (command)
//   <node>/player/loop          "true" / "false" (state, retained)
//   <node>/player/loop/set      "on"/"off", "true"/"false", "1"/"0"
//   <node>/player/status        "bereit", "spielt <file>", "fertig",
//                               "gestoppt" or "fehler: <file> <reason>"
//
// Status needs no request command: the framework republishes all device
// values every transmission_delta (default 5000 ms, see main.cpp), so a
// fresh subscriber gets the current status within a few seconds anyway.
//
// Fades are 0 - 60000 ms; 0 means only the short anti-click fade at the start
// of a file. Volume, loop and the fades are retained, so a fresh subscriber
// (dashboard, Node-RED) knows the current configuration right away. Status is
// not retained: a retained "spielt x.wav" would stay stale after the playback
// ended. Use the "state" command to refresh all values instead.
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
#define I2S_PLAYER_SUB_FADE_IN 4
#define I2S_PLAYER_SUB_FADE_IN_SET 5
#define I2S_PLAYER_SUB_FADE_OUT 6
#define I2S_PLAYER_SUB_FADE_OUT_SET 7
#define I2S_PLAYER_SUB_STATUS 8
#define I2S_PLAYER_SUB_LOOP 9
#define I2S_PLAYER_SUB_LOOP_SET 10

#ifdef ESP32

#include "i2s_player_engine.h"

class I2S_Player : public Device {
    private:
        I2S_Player_Engine _engine;
        I2S_Player_Engine::Pins _pins;
        float _volume = 0.5f;
        bool _loop = false;
        uint16_t _fade_in_ms = 0;
        uint16_t _fade_out_ms = 0;
        uint8_t _status_seq = 0;

        void set_volume(float volume);
        void set_loop(bool loop);
        void set_fade_in(uint16_t ms);
        void set_fade_out(uint16_t ms);

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
