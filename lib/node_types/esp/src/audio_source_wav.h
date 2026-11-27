// audio_source_wav.h
// 16 bit PCM WAV decoder behind the Audio_Source interface. It reads through
// an Audio_Storage, so the same decoder serves the SD card and, later, the
// internal flash.
//
// The play payload is a file name; how it is resolved (folder prefix, medium)
// is up to the storage device. The decoder knows the RIFF layout only.
//
// Supported: RIFF/WAVE, PCM (also WAVE_FORMAT_EXTENSIBLE with a PCM sub
// format), 16 bit, 1 or 2 channels. The sample rate comes from the header.

#ifndef _AUDIO_SOURCE_WAV_H_
#define _AUDIO_SOURCE_WAV_H_

#include "audio_source.h"
#include "audio_storage.h"

#ifdef ESP32

class Audio_Source_Wav : public Audio_Source {
    public:
        explicit Audio_Source_Wav(Audio_Storage* storage = NULL)
                : _storage(storage) {}

        bool open(const char* payload);
        uint32_t sample_rate() const { return _sample_rate; }
        uint8_t channels() const { return _channels; }
        int read(int16_t* frames, size_t want);
        bool rewind();
        bool loopable() const { return true; }
        void close();

    private:
        bool read_header();
        uint16_t read_u16();
        uint32_t read_u32();

        Audio_Storage* _storage;
        Audio_File* _file = NULL;
        uint32_t _sample_rate = 0;
        uint8_t _channels = 0;
        uint32_t _data_offset = 0;
        uint32_t _data_size = 0;
        uint32_t _remaining = 0;
};

#endif // ESP32

#endif // _AUDIO_SOURCE_WAV_H_
