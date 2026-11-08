// dev_i2s_player.cpp
// I2S audio player device: plays 16 bit PCM WAV files from an SD card.

#include "dev_i2s_player.h"

#ifdef ESP32

#include <string.h>

#include <toolbox.h>

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
            _engine.play(file.as_cstr());
            return true;
        }
    );
    add_subdevice(new Subdevice(F("stop/set"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            (void)payload; // any payload means stop
            _engine.stop();
            return true;
        }
    );
    add_subdevice(new Subdevice(F("volume")));
    add_subdevice(new Subdevice(F("volume/set"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            set_volume(payload.as_float());
            return true;
        }
    );
    add_subdevice(new Subdevice(F("status")));
    add_subdevice(new Subdevice(F("loop")));
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

void I2S_Player::start() {
    _engine.set_volume(_volume);
    _engine.set_loop(_loop);
    value(I2S_PLAYER_SUB_VOLUME).from(_volume);
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
