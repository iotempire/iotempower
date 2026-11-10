// audio_storage.h
// Interface between a storage medium (SD card now, the internal flash later)
// and the WAV decoder. It carries no Arduino types, so the decoder does not
// need to know which medium the samples come from.

#ifndef _AUDIO_STORAGE_H_
#define _AUDIO_STORAGE_H_

#include <stdint.h>
#include <stddef.h>

#ifdef ESP32

// An open file. The storage device owns the object; the decoder only borrows
// the pointer between open() and close().
class Audio_File {
    public:
        virtual ~Audio_File() {}

        // > 0 bytes read, 0 = end of file, < 0 = read error.
        virtual int read(void* buffer, size_t bytes) = 0;

        // Absolute byte position from the start of the file.
        virtual bool seek(uint32_t position) = 0;
        virtual uint32_t position() = 0;
        virtual uint32_t size() = 0;

        virtual void close() = 0;
};

class Audio_Storage {
    public:
        virtual ~Audio_Storage() {}

        // Resolves name on the medium and returns the file object or NULL.
        // Naming (folder prefix, case sensitivity) is the medium's business.
        virtual Audio_File* open(const char* name) = 0;
};

#endif // ESP32

#endif // _AUDIO_STORAGE_H_
