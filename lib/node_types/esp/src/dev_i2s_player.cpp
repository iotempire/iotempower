// dev_i2s_player.cpp
// I2S audio player device: plays 16 bit PCM WAV files from an SD card.
//
// All MQTT payloads are single values (file name, number, on/off). The device
// keeps volume, loop and both fade times as its own configuration; a play
// command uses those values as they are at that moment.

#include "dev_i2s_player.h"

#ifdef ESP32

#include <string.h>

#include <toolbox.h>

// Longest fade accepted from MQTT. One minute is far beyond anything useful
// and keeps the values inside the uint16_t range used by the engine.
#define I2S_PLAYER_MAX_FADE_MS 60000

static uint16_t clamp_fade(long ms) {
    if (ms < 0) return 0;
    if (ms > I2S_PLAYER_MAX_FADE_MS) return I2S_PLAYER_MAX_FADE_MS;
    return (uint16_t)ms;
}

I2S_Player::I2S_Player(const char* name,
        uint8_t sd_cs, uint8_t sd_sck, uint8_t sd_mosi, uint8_t sd_miso,
        uint8_t i2s_bclk, uint8_t i2s_lrck, uint8_t i2s_data)
        : Device(name, 200000) { // only a status value is polled, 200 ms is plenty
    _pins.sd_cs = sd_cs;
    _pins.sd_sck = sd_sck;
    _pins.sd_mosi = sd_mosi;
    _pins.sd_miso = sd_miso;
    _pins.i2s_bclk = i2s_bclk;
    _pins.i2s_lrck = i2s_lrck;
    _pins.i2s_data = i2s_data;

    // The framework lowercases incoming payloads by default. File names on
    // the card keep their case, so this must stay off.
    with_ignore_case(false);

    add_subdevice(new Subdevice(F("play/set"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            Ustring file(payload);
            file.strip();
            if (file.empty()) {
                ulog(F("i2s_player: empty play command ignored."));
                return true;
            }
            // -1 keeps the current base volume; the fades are the ones
            // configured at this moment.
            _engine.play(file.as_cstr(), -1.0f, _fade_in_ms, _fade_out_ms);
            return true;
        }
    );
    add_subdevice(new Subdevice(F("stop/set"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            // Optional fade time in ms. Without a payload the engine uses its
            // short interrupt fade, which keeps a stop responsive.
            Ustring text(payload);
            text.strip();
            _engine.stop(text.empty() ? 0 : clamp_fade(text.as_int()));
            return true;
        }
    );
    // Configuration values: published retained, so a dashboard or Node-RED
    // knows them right after a restart.
    add_subdevice(new Subdevice(F("volume")))->with_retained(true);
    add_subdevice(new Subdevice(F("volume/set"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            set_volume(payload.as_float());
            return true;
        }
    );
    add_subdevice(new Subdevice(F("fade_in")))->with_retained(true);
    add_subdevice(new Subdevice(F("fade_in/set"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            set_fade_in(clamp_fade(payload.as_int()));
            return true;
        }
    );
    add_subdevice(new Subdevice(F("fade_out")))->with_retained(true);
    add_subdevice(new Subdevice(F("fade_out/set"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            set_fade_out(clamp_fade(payload.as_int()));
            return true;
        }
    );
    // Status is a plain state subdevice. It needs no command topic on top:
    // the framework republishes every value every transmission_delta
    // (default 5 s), so a fresh subscriber sees the current state anyway.
    // (A subscribed "status" would also receive the device's own status
    // messages: the subscribe topic of a subdevice is its own name, with no
    // automatic "/set" suffix.)
    add_subdevice(new Subdevice(F("status")));
    add_subdevice(new Subdevice(F("loop")))->with_retained(true);
    add_subdevice(new Subdevice(F("loop/set"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            set_loop(payload.equals(F("on")) || payload.equals(F("true"))
                || payload.equals(F("1")));
            return true;
        }
    );
}

void I2S_Player::set_volume(float volume) {
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    _volume = volume;
    _engine.set_volume(volume);
    value(I2S_PLAYER_SUB_VOLUME).from(volume);
}

void I2S_Player::set_loop(bool loop) {
    _loop = loop;
    _engine.set_loop(loop);
    value(I2S_PLAYER_SUB_LOOP).from(loop ? F("true") : F("false"));
}

void I2S_Player::set_fade_in(uint16_t ms) {
    _fade_in_ms = ms;
    value(I2S_PLAYER_SUB_FADE_IN).from((unsigned long)ms);
}

void I2S_Player::set_fade_out(uint16_t ms) {
    _fade_out_ms = ms;
    value(I2S_PLAYER_SUB_FADE_OUT).from((unsigned long)ms);
}

void I2S_Player::start() {
    _engine.set_volume(_volume);
    _engine.set_loop(_loop);
    value(I2S_PLAYER_SUB_VOLUME).from(_volume);
    value(I2S_PLAYER_SUB_FADE_IN).from((unsigned long)_fade_in_ms);
    value(I2S_PLAYER_SUB_FADE_OUT).from((unsigned long)_fade_out_ms);
    value(I2S_PLAYER_SUB_LOOP).from(_loop ? F("true") : F("false"));

    if (!_engine.begin(_pins)) {
        // Leave _started false: without I2S the device would only report
        // errors and nothing can be played.
        ulog(F("i2s_player: I2S initialisation failed, device stays inactive."));
        return;
    }
    _started = true;
}

bool I2S_Player::measure() {
    char status[I2S_PLAYER_STATUS_STRLEN];
    if (_engine.copy_status(status, sizeof(status), _status_seq)) {
        value(I2S_PLAYER_SUB_STATUS).from(status);
        return true;
    }
    return false;
}

#endif // ESP32
