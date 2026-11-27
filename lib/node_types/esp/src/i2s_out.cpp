// i2s_out.cpp

#include "i2s_out.h"

#ifdef ESP32

#include <driver/i2s.h>
#include <freertos/FreeRTOS.h>

#include <toolbox.h>

#define I2S_OUT_PORT ((i2s_port_t)0)

// Output format. Stereo is the default, matching the design decision
// (LOUT/ROUT of the DAC each carry one channel). Define
// I2S_PLAYER_MONO_OUT to fold both channels into one mono stream
// (I2S_CHANNEL_FMT_ALL_LEFT), which is the variant verified in the audio2
// prototype. On ESP32-S2 the stereo slot layout was silent in that setup, so
// try mono first if this device stays quiet.
#ifndef I2S_PLAYER_MONO_OUT
#define I2S_PLAYER_MONO_OUT 0
#endif

#if I2S_PLAYER_MONO_OUT
#define I2S_OUT_NAME "mono"
#define I2S_OUT_CLOCK_CHANNELS I2S_CHANNEL_MONO
#define I2S_OUT_SLOT_FORMAT I2S_CHANNEL_FMT_ALL_LEFT
#else
#define I2S_OUT_NAME "stereo"
#define I2S_OUT_CLOCK_CHANNELS I2S_CHANNEL_STEREO
#define I2S_OUT_SLOT_FORMAT I2S_CHANNEL_FMT_RIGHT_LEFT
#endif

void I2S_Out::set_pins(uint8_t bclk, uint8_t lrck, uint8_t data) {
    _bclk = bclk;
    _lrck = lrck;
    _data = data;
}

bool I2S_Out::install(uint32_t rate) {
    i2s_config_t config = {};
    config.mode = (i2s_mode_t)(I2S_MODE_TX | I2S_MODE_MASTER);
    config.sample_rate = rate;
    config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    config.channel_format = I2S_OUT_SLOT_FORMAT;
    config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
    config.dma_buf_count = 8;
    config.dma_buf_len = 256;
    config.use_apll = true;
    config.tx_desc_auto_clear = true;
    config.fixed_mclk = 0;

    if (i2s_driver_install(I2S_OUT_PORT, &config, 0, NULL) != ESP_OK) {
        ulog(F("i2s_out: installing the I2S driver failed."));
        return false;
    }
    i2s_pin_config_t pin_config = {};
    pin_config.bck_io_num = _bclk;
    pin_config.ws_io_num = _lrck;
    pin_config.data_out_num = _data;
    pin_config.data_in_num = I2S_PIN_NO_CHANGE;
    if (i2s_set_pin(I2S_OUT_PORT, &pin_config) != ESP_OK) {
        ulog(F("i2s_out: assigning the I2S pins failed."));
        return false;
    }
    if (i2s_set_clk(I2S_OUT_PORT, rate, I2S_BITS_PER_SAMPLE_16BIT,
            I2S_OUT_CLOCK_CHANNELS) != ESP_OK) {
        ulog(F("i2s_out: setting the I2S clock failed."));
        return false;
    }
    i2s_start(I2S_OUT_PORT);
    _rate = rate;
    _installed = true;
    ulog(F("i2s_out: ready (%s, %u Hz, BCLK %u, LRCK %u, DATA %u)."),
        I2S_OUT_NAME, (unsigned)rate, (unsigned)_bclk, (unsigned)_lrck,
        (unsigned)_data);
    return true;
}

bool I2S_Out::set_rate(uint32_t rate) {
    if (rate == 0) return false;
    if (!_installed) return install(rate);
    if (rate == _rate) return true;
    if (i2s_set_clk(I2S_OUT_PORT, rate, I2S_BITS_PER_SAMPLE_16BIT,
            I2S_OUT_CLOCK_CHANNELS) != ESP_OK) {
        ulog(F("i2s_out: cannot change the sample rate to %u Hz."),
            (unsigned)rate);
        return false;
    }
    _rate = rate;
    return true;
}

size_t I2S_Out::write(int16_t* frames, size_t frame_count,
        uint32_t timeout_ms) {
    if (!_installed || frames == NULL || frame_count == 0) return 0;
#if I2S_PLAYER_MONO_OUT
    // Downmix in place: the write index i never overtakes the read index
    // 2 * i, so a forward loop is safe.
    for (size_t i = 0; i < frame_count; i++) {
        frames[i] = (int16_t)(((int32_t)frames[2 * i]
            + (int32_t)frames[2 * i + 1]) / 2);
    }
    size_t total = frame_count * 2;
#else
    size_t total = frame_count * 4;
#endif
    size_t offset = 0;
    int failures = 0;
    while (offset < total) {
        size_t written = 0;
        esp_err_t rc = i2s_write(I2S_OUT_PORT, (uint8_t*)frames + offset,
            total - offset, &written, pdMS_TO_TICKS(timeout_ms));
        if (rc != ESP_OK || written == 0) {
            // Do not flood the log when the audio output is broken.
            if (failures < 3) {
                failures++;
                ulog(F("i2s_out: I2S write failed (rc=%d, %u of %u bytes)."),
                    (int)rc, (unsigned)written, (unsigned)(total - offset));
            }
            break;
        }
        offset += written;
    }
    return offset;
}

void I2S_Out::silence() {
    if (_installed) i2s_zero_dma_buffer(I2S_OUT_PORT);
}

#endif // ESP32
