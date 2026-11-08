// i2s_player_wav.cpp
// Minimal RIFF/PCM WAV header reader for the i2s_player device.

#include "i2s_player_wav.h"

#ifdef ESP32

#include <string.h>

static uint16_t read_u16(File& f) {
    uint8_t b[2] = {0, 0};
    f.read(b, 2);
    return (uint16_t)(b[0] | ((uint16_t)b[1] << 8));
}

static uint32_t read_u32(File& f) {
    uint8_t b[4] = {0, 0, 0, 0};
    f.read(b, 4);
    return (uint32_t)b[0] | ((uint32_t)b[1] << 8) |
           ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}

bool i2s_wav_read_info(File& f, I2S_Wav_Info& info) {
    info = I2S_Wav_Info();
    if (!f.seek(0)) return false;

    char id[4];
    if (f.read((uint8_t*)id, 4) != 4 || memcmp(id, "RIFF", 4) != 0) return false;
    read_u32(f);                                              // RIFF size (unused)
    if (f.read((uint8_t*)id, 4) != 4 || memcmp(id, "WAVE", 4) != 0) return false;

    bool have_fmt = false;
    while (f.available() >= 8) {
        size_t chunk_start = f.position();
        if (f.read((uint8_t*)id, 4) != 4) break;
        uint32_t size = read_u32(f);
        if (size == 0 && memcmp(id, "data", 4) != 0) break;   // end or truncated

        if (memcmp(id, "fmt ", 4) == 0) {
            if (size < 16) return false;
            uint16_t format = read_u16(f);
            info.channels = read_u16(f);
            info.sample_rate = read_u32(f);
            read_u32(f);                                      // byte rate
            read_u16(f);                                      // block align
            info.bits = read_u16(f);
            if (format == 0xFFFE && size >= 40) {             // WAVE_FORMAT_EXTENSIBLE
                f.seek(chunk_start + 8 + 24);
                format = read_u16(f);                         // sub format tag
            }
            if (format != 1) return false;                    // PCM only
            if (info.channels == 0 || info.sample_rate == 0) return false;
            have_fmt = true;
        } else if (memcmp(id, "data", 4) == 0) {
            if (!have_fmt) return false;
            info.data_offset = f.position();
            info.data_size = size;
            return true;
        }

        size_t next = chunk_start + 8 + size + (size & 1);     // chunks are word aligned
        if (next <= chunk_start) return false;                 // guard against endless loop
        if (!f.seek(next)) break;
    }
    return false;
}

#endif // ESP32
