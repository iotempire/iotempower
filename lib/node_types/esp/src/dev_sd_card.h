// dev_sd_card.h
// Generic SD card device: owns the SPI bus and the card mount, so other
// devices can read and write files. Declare it before the devices that use it
// and pass it with IN():
//
//   sd_card(sd, 2, 4, 6, 8);      // CS, SCK, MOSI, MISO
//   sd_audio(quelle, IN(sd));
//
// Topic: <node>/sd/status -> unmounted | mounted | error: sd mount
// (state, retained; the instance name in setup.cpp is part of the topic).
//
// Writing from another device (for example a logger):
//   File log = IN(sd).open("/log/data.csv", FILE_APPEND);
//   if (log) log.println(...);
//
// The card is mounted in start() and retried on every open(), so a node
// without a card still boots and recovers as soon as the card appears.

#ifndef _SD_CARD_H_
#define _SD_CARD_H_

#include <device.h>

#ifdef ESP32

#include <FS.h>
#include <SD.h>
#include <SPI.h>

class SD_Card : public Device {
    public:
        SD_Card(const char* name, uint8_t cs, uint8_t sck, uint8_t mosi,
            uint8_t miso, uint32_t frequency = 4000000);
        void start();
        bool measure();

        // Idempotent; returns false if no card could be mounted.
        bool mount();
        bool mounted();

        // File access for other devices. Paths start with "/" and are
        // relative to the card root ("/audio/x.wav"); Arduino SD prepends its
        // mount point (/sd) internally.
        File open(const char* path, const char* mode = FILE_READ);
        bool exists(const char* path);
        bool remove(const char* path);
        bool mkdir(const char* path);

        // Bytes on the card (0 if no card is mounted).
        uint64_t size();
        uint64_t used();
        uint64_t free();

    private:
        enum State : uint8_t { STATE_UNMOUNTED = 0, STATE_MOUNTED, STATE_ERROR };

        uint8_t _cs;
        uint8_t _sck;
        uint8_t _mosi;
        uint8_t _miso;
        uint32_t _frequency;
        bool _mounted = false;
        // Written from start() and from whichever task opens a file, published
        // only in measure() (main loop).
        volatile uint8_t _state = STATE_UNMOUNTED;
        uint8_t _published_state = STATE_UNMOUNTED;
};

#else

// The card exists only on ESP32. The type stays compilable elsewhere.
class SD_Card : public Device {
    public:
        SD_Card(const char* name, uint8_t, uint8_t, uint8_t, uint8_t,
            uint32_t = 4000000) : Device(name, 1000000) {}
};

#endif // ESP32

#endif // _SD_CARD_H_
