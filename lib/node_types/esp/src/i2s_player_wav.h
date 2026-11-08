// i2s_player_wav.h
// Minimal RIFF/PCM WAV header reader for the i2s_player device.

#ifndef _I2S_PLAYER_WAV_H_
#define _I2S_PLAYER_WAV_H_

#include <Arduino.h>

#ifdef ESP32

#include <SD.h>

// Properties read from the RIFF header of a PCM WAV file.
struct I2S_Wav_Info {
    uint32_t data_offset = 0;  // file position of the first PCM byte
    uint32_t data_size = 0;    // length of the PCM data in bytes
    uint32_t sample_rate = 0;
    uint16_t channels = 0;
    uint16_t bits = 0;
};

// Reads the header and positions f at the start of the data chunk.
// Returns false if the file is not a PCM WAV file.
bool i2s_wav_read_info(File& f, I2S_Wav_Info& info);

#endif // ESP32

#endif // _I2S_PLAYER_WAV_H_
