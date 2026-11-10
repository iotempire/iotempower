// i2s_out.h
// Minimal I2S output sink: install the driver, set the sample rate, write
// interleaved 16 bit frames, stop. No volume, no files, no MQTT - that is the
// player's business.

#ifndef _I2S_OUT_H_
#define _I2S_OUT_H_

#include <stdint.h>
#include <stddef.h>

#ifdef ESP32

class I2S_Out {
    public:
        void set_pins(uint8_t bclk, uint8_t lrck, uint8_t data);

        // Installs the driver on first use, reconfigures the clock afterwards.
        // The sample rate becomes known only with the first playback.
        bool set_rate(uint32_t rate);

        // Frames are interleaved stereo. With I2S_PLAYER_MONO_OUT the two
        // channels are mixed down in place before writing.
        size_t write(int16_t* frames, size_t frame_count, uint32_t timeout_ms);

        // Drop the DMA buffers (used on stop).
        void silence();

        bool installed() const { return _installed; }

    private:
        bool install(uint32_t rate);

        uint8_t _bclk = 0;
        uint8_t _lrck = 0;
        uint8_t _data = 0;
        uint32_t _rate = 0;
        bool _installed = false;
};

#endif // ESP32

#endif // _I2S_OUT_H_
