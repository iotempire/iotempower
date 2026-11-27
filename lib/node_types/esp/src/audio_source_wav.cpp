// audio_source_wav.cpp

#include "audio_source_wav.h"

#ifdef ESP32

#include <string.h>

#include <toolbox.h>

#include "audio_status.h"

bool Audio_Source_Wav::open(const char* payload) {
    close();
    if (_storage == NULL) {
        ulog(F("i2s_wav: no storage medium registered."));
        audio_status_error("error: no storage");
        return false;
    }
    if (payload == NULL || payload[0] == 0) {
        ulog(F("i2s_wav: play needs a file name."));
        audio_status_error("error: no file name");
        return false;
    }

    _file = _storage->open(payload);
    // The storage is responsible for the message (missing card, missing file);
    // the generic text is only a fallback for media without their own report.
    if (_file == NULL) {
        if (!audio_status_has_error()) audio_status_error("error: open");
        return false;
    }

    if (!read_header() || !rewind()) {
        close();
        return false;
    }

    ulog(F("i2s_wav: >%s< ready (%u Hz, %u channel(s), %u bytes)."),
        payload, (unsigned)_sample_rate, (unsigned)_channels,
        (unsigned)_data_size);
    return true;
}

uint16_t Audio_Source_Wav::read_u16() {
    uint8_t b[2] = {0, 0};
    if (_file->read(b, 2) != 2) return 0;
    return (uint16_t)(b[0] | ((uint16_t)b[1] << 8));
}

uint32_t Audio_Source_Wav::read_u32() {
    uint8_t b[4] = {0, 0, 0, 0};
    if (_file->read(b, 4) != 4) return 0;
    return (uint32_t)b[0] | ((uint32_t)b[1] << 8) |
           ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}

bool Audio_Source_Wav::read_header() {
    _sample_rate = 0;
    _channels = 0;
    _data_offset = 0;
    _data_size = 0;

    if (!_file->seek(0)) return false;

    char id[4];
    if (_file->read(id, 4) != 4 || memcmp(id, "RIFF", 4) != 0) {
        ulog(F("i2s_wav: missing RIFF header."));
        audio_status_error("error: not a wav");
        return false;
    }
    read_u32(); // RIFF size, unused
    if (_file->read(id, 4) != 4 || memcmp(id, "WAVE", 4) != 0) {
        ulog(F("i2s_wav: not a WAVE file."));
        audio_status_error("error: not a wav");
        return false;
    }

    const uint32_t file_size = _file->size();
    bool have_fmt = false;
    while (_file->position() + 8 <= file_size) {
        const uint32_t chunk_start = _file->position();
        if (_file->read(id, 4) != 4) break;
        const uint32_t size = read_u32();

        if (memcmp(id, "fmt ", 4) == 0) {
            if (size < 16) return false;
            uint16_t format = read_u16();
            const uint16_t channels = read_u16();
            const uint32_t rate = read_u32();
            read_u32(); // byte rate
            read_u16(); // block align
            const uint16_t bits = read_u16();
            if (format == 0xFFFE && size >= 40) { // WAVE_FORMAT_EXTENSIBLE
                if (!_file->seek(chunk_start + 8 + 24)) return false;
                format = read_u16(); // sub format tag
            }
            if (format != 1 || bits != 16 || channels < 1 || channels > 2
                    || rate == 0) {
                ulog(F("i2s_wav: unsupported format (format %u, %u bit, "
                    "%u channel(s))."), (unsigned)format, (unsigned)bits,
                    (unsigned)channels);
                audio_status_error("error: unsupported format");
                return false;
            }
            _channels = (uint8_t)channels;
            _sample_rate = rate;
            have_fmt = true;
        } else if (memcmp(id, "data", 4) == 0) {
            if (!have_fmt) {
                ulog(F("i2s_wav: data chunk before fmt chunk."));
                audio_status_error("error: not a wav");
                return false;
            }
            _data_offset = _file->position();
            _data_size = size;
            if (_data_offset >= file_size) {
                ulog(F("i2s_wav: no audio data."));
                audio_status_error("error: empty");
                return false;
            }
            // A truncated or lying header must not make us read past the end.
            if (_data_size > file_size - _data_offset) {
                _data_size = file_size - _data_offset;
            }
            if (_data_size < (uint32_t)_channels * 2u) {
                ulog(F("i2s_wav: no audio data."));
                audio_status_error("error: empty");
                return false;
            }
            return true;
        }

        // Chunks are word aligned. A size that does not advance the position
        // would loop forever, so guard against it.
        const uint32_t next = chunk_start + 8 + size + (size & 1u);
        if (next <= chunk_start || next >= file_size) break;
        if (!_file->seek(next)) break;
    }
    ulog(F("i2s_wav: no usable fmt/data chunk."));
    audio_status_error("error: not a wav");
    return false;
}

int Audio_Source_Wav::read(int16_t* frames, size_t want) {
    if (_file == NULL || frames == NULL || want == 0) return 0;

    const uint32_t block_align = (uint32_t)_channels * 2u;
    if (_remaining < block_align) return 0;

    uint32_t bytes = (uint32_t)want * block_align;
    if (bytes > _remaining) bytes = _remaining;

    const int got = _file->read(frames, bytes);
    if (got <= 0) {
        // A read error also ends the stream: otherwise the player would wait
        // forever for data that will never come.
        if (got < 0) {
            ulog(F("i2s_wav: read error."));
            audio_status_error("error: read");
        }
        return 0;
    }

    const uint32_t used = ((uint32_t)got / block_align) * block_align;
    if (used != (uint32_t)got) {
        // A short read ended inside a frame; leave the rest for the next call.
        _file->seek(_file->position() - ((uint32_t)got - used));
    }
    _remaining -= used;
    return (int)(used / block_align);
}

bool Audio_Source_Wav::rewind() {
    if (_file == NULL || !_file->seek(_data_offset)) {
        ulog(F("i2s_wav: cannot seek to the audio data."));
        audio_status_error("error: read");
        return false;
    }
    _remaining = _data_size;
    return true;
}

void Audio_Source_Wav::close() {
    if (_file != NULL) {
        _file->close();
        _file = NULL;
    }
    _remaining = 0;
    _data_size = 0;
    _data_offset = 0;
}

#endif // ESP32
