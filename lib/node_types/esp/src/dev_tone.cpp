// dev_tone.cpp

#include "dev_tone.h"

namespace {
    const int TONE_SUBDEVICE_STATUS = 4;
    const int TONE_SUBDEVICE_PLAY = 5;

    const int C4_FREQUENCIES[12] = {
        262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494
    };
}

Tone::Tone(const char* name, uint8_t pin, int frequency)
        : Pwm(name, pin, frequency) {
    set_duty(_melody_duty);

    add_subdevice(new Subdevice(F("status")));
    add_subdevice(new Subdevice(F("play"), true))->with_receive_cb(
        [&] (const Ustring& payload) {
            Ustring command(payload);
            command.strip();
            if(command.equals(F("stop"), true)) {
                stop_playback();
                return true;
            }
            if(command.empty()) return true;
            begin_playback(command);
            return true;
        }
    );

    value(TONE_SUBDEVICE_STATUS).from(F("stopped"));
}

void Tone::start() {
    Pwm::start();
    stop_playback();
}

bool Tone::measure() {
    int configured_duty = value(0).as_int();
    if(configured_duty > 0 && configured_duty <= 1023) {
        _melody_duty = configured_duty;
    }
    if(_playing) {
        unsigned long now = millis();
        if(now - _next_note_at < 0x80000000UL) {
            schedule_next_note();
        }
    }
    return true;
}

void Tone::stop_playback() {
    _playing = false;
    set_duty(0);
    value(TONE_SUBDEVICE_STATUS).from(F("stopped"));
}

bool Tone::begin_playback(const Ustring& melody) {
    _melody.copy(melody);
    _default_duration = 4;
    _default_octave = 6;
    _bpm = 120;
    _notes_start = 0;

    const char* text = _melody.as_cstr();
    int len = _melody.length();
    int first_colon = -1;
    int second_colon = -1;
    for(int i = 0; i < len; i++) {
        if(text[i] == ':') {
            if(first_colon < 0) first_colon = i;
            else {
                second_colon = i;
                break;
            }
        }
    }

    if(first_colon >= 0 && second_colon > first_colon) {
        Ustring defaults;
        defaults.from(text + first_colon + 1, second_colon - first_colon - 1);
        if(!parse_defaults(defaults.as_cstr())) {
            stop_playback();
            return false;
        }
        _notes_start = second_colon + 1;
    } else if(first_colon < 0) {
        _notes_start = 0;
    } else {
        stop_playback();
        return false;
    }

    _cursor = _notes_start;
    _playing = true;
    value(TONE_SUBDEVICE_STATUS).from(F("playing"));
    schedule_next_note();
    return true;
}

bool Tone::parse_defaults(const char* defaults) {
    const char* p = defaults;
    while(*p) {
        while(*p && (is_space(*p) || *p == ',')) p++;
        if(!*p) break;

        char key = tolower(*p++);
        while(*p && is_space(*p)) p++;
        if(*p != '=') return false;
        p++;
        while(*p && is_space(*p)) p++;

        const char* value_start = p;
        while(*p && *p != ',') p++;
        int value_len = p - value_start;
        if(value_len <= 0) return false;

        Ustring value;
        value.from(value_start, value_len);
        value.strip();

        if(key == 'd') {
            int duration = value.as_int();
            if(duration < 1 || duration > 64) return false;
            _default_duration = duration;
        } else if(key == 'o') {
            int octave = value.as_int();
            if(octave < 1 || octave > 8) return false;
            _default_octave = octave;
        } else if(key == 'b') {
            int bpm = value.as_int();
            if(bpm < 20 || bpm > 900) return false;
            _bpm = bpm;
        } else if(key == 'k') {
            if(!parse_key(value.as_cstr())) return false;
        } else {
            return false;
        }
    }
    return true;
}

bool Tone::parse_key(const char* key) {
    Ustring key_u(key);
    key_u.lower();
    key_u.strip();
    if(key_u.empty()) return false;

    int mode_start = key_u.find(F("maj"));
    if(mode_start < 0) mode_start = key_u.find(F("min"));
    if(mode_start < 0 && key_u.length() >= 2 && key_u.as_cstr()[key_u.length() - 1] == 'm') {
        mode_start = key_u.length() - 1;
    }
    if(mode_start < 0) return false;
    return true;
}

void Tone::schedule_next_note() {
    int frequency = 0;
    unsigned long duration_ms = 0;
    if(!parse_next_note(frequency, duration_ms)) {
        stop_playback();
        return;
    }

    if(frequency <= 0) {
        set_duty(0);
    } else {
        set(_melody_duty, frequency);
    }

    _next_note_at = millis() + duration_ms;
}

bool Tone::parse_next_note(int& frequency, unsigned long& duration_ms) {
    const char* text = _melody.as_cstr();
    int len = _melody.length();

    while(_cursor < len && (is_space(text[_cursor]) || text[_cursor] == ',')) _cursor++;
    if(_cursor >= len) return false;

    int duration = 0;
    while(_cursor < len && is_digit(text[_cursor])) {
        duration = duration * 10 + (text[_cursor] - '0');
        _cursor++;
    }
    if(duration <= 0) duration = _default_duration;

    if(_cursor >= len) return false;
    char note = tolower(text[_cursor++]);
    if(!(note == 'c' || note == 'd' || note == 'e' || note == 'f'
        || note == 'g' || note == 'a' || note == 'b' || note == 'p')) {
        return false;
    }

    bool sharp = false;
    if(_cursor < len && text[_cursor] == '#') {
        sharp = true;
        _cursor++;
    }

    int octave = _default_octave;
    if(_cursor < len && is_digit(text[_cursor])) {
        octave = text[_cursor] - '0';
        _cursor++;
    }

    bool dotted = false;
    if(_cursor < len && text[_cursor] == '.') {
        dotted = true;
        _cursor++;
    }

    while(_cursor < len && text[_cursor] != ',') {
        if(!is_space(text[_cursor])) return false;
        _cursor++;
    }
    if(_cursor < len && text[_cursor] == ',') _cursor++;

    unsigned long whole_note_ms = (unsigned long) 240000L / (unsigned long)_bpm;
    duration_ms = whole_note_ms / (unsigned long)duration;
    if(dotted) duration_ms += duration_ms / 2;
    if(duration_ms == 0) duration_ms = 1;

    if(note == 'p') {
        frequency = 0;
    } else {
        frequency = note_to_frequency(note, sharp, octave);
        if(frequency < 2 || frequency > 5000) return false;
    }
    return true;
}

int Tone::note_to_frequency(char note, bool sharp, int octave) {
    int semitone = 0;
    switch(note) {
        case 'c': semitone = 0; break;
        case 'd': semitone = 2; break;
        case 'e': semitone = 4; break;
        case 'f': semitone = 5; break;
        case 'g': semitone = 7; break;
        case 'a': semitone = 9; break;
        case 'b': semitone = 11; break;
        default: return 0;
    }
    if(sharp) {
        semitone++;
        if(semitone > 11) semitone = 11;
    }

    int freq = C4_FREQUENCIES[semitone];
    octave = limit(octave, 1, 8);
    if(octave > 4) {
        freq <<= (octave - 4);
    } else if(octave < 4) {
        freq >>= (4 - octave);
    }
    return limit(freq, 2, 5000);
}

bool Tone::is_space(char c) {
    return c <= 32;
}

bool Tone::is_digit(char c) {
    return c >= '0' && c <= '9';
}
