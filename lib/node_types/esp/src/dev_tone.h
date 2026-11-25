// tone.h
// play melodies on a pwm-controlled buzzer

#ifndef _TONE_H_
#define _TONE_H_

#include <dev_pwm.h>

class Tone : public Pwm {
    private:
        Ustring _melody;
        int _notes_start = 0;
        int _cursor = 0;
        int _default_duration = 4;
        int _default_octave = 6;
        int _bpm = 120;
        int _melody_duty = 512;
        bool _playing = false;
        unsigned long _next_note_at = 0;
        bool parse_defaults(const char* defaults);
        bool parse_key(const char* key);
        bool begin_playback(const Ustring& melody);
        bool parse_next_note(int& frequency, unsigned long& duration_ms);
        int note_to_frequency(char note, bool sharp, int octave);
        void schedule_next_note();
        void stop_playback();
        static bool is_space(char c);
        static bool is_digit(char c);
    public:
        Tone(const char* name, uint8_t pin, int frequency = 1000);
        bool measure() override;
        void start();
};

#endif // _TONE_H_
