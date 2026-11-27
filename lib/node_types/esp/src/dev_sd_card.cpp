// dev_sd_card.cpp

#include "dev_sd_card.h"

#ifdef ESP32

#include <toolbox.h>

SD_Card::SD_Card(const char* name, uint8_t cs, uint8_t sck, uint8_t mosi,
        uint8_t miso, uint32_t frequency)
        : Device(name, 500000), // measure() only publishes the mount state
          _cs(cs), _sck(sck), _mosi(mosi), _miso(miso), _frequency(frequency) {
    add_subdevice(new Subdevice(F("status")))->with_retained(true);
    value(0).from(F("unmounted"));
}

void SD_Card::start() {
    mount();
    // Stay started even without a card: the status topic is what reports the
    // problem, and every open() retries the mount.
    _started = true;
}

bool SD_Card::mount() {
    if (_mounted) return true;
    // The board has no usable default SPI pins, so they are set explicitly
    // before SD.begin() takes over the bus.
    SPI.begin(_sck, _miso, _mosi, _cs);
    if (!SD.begin(_cs, SPI, _frequency)) {
        ulog(F("sd_card: mounting the card failed "
            "(CS %u, SCK %u, MOSI %u, MISO %u)."), (unsigned)_cs,
            (unsigned)_sck, (unsigned)_mosi, (unsigned)_miso);
        _state = STATE_ERROR;
        return false;
    }
    _mounted = true;
    _state = STATE_MOUNTED;
    ulog(F("sd_card: card mounted (CS %u, SCK %u, MOSI %u, MISO %u)."),
        (unsigned)_cs, (unsigned)_sck, (unsigned)_mosi, (unsigned)_miso);
    return true;
}

bool SD_Card::mounted() {
    return _mounted;
}

File SD_Card::open(const char* path, const char* mode) {
    if (path == NULL || !mount()) return File();
    return SD.open(path, mode);
}

bool SD_Card::exists(const char* path) {
    return path != NULL && mount() && SD.exists(path);
}

bool SD_Card::remove(const char* path) {
    return path != NULL && mount() && SD.remove(path);
}

bool SD_Card::mkdir(const char* path) {
    return path != NULL && mount() && SD.mkdir(path);
}

uint64_t SD_Card::size() {
    return mount() ? SD.cardSize() : 0;
}

uint64_t SD_Card::used() {
    return mount() ? SD.usedBytes() : 0;
}

uint64_t SD_Card::free() {
    if (!mount()) return 0;
    const uint64_t total = SD.totalBytes();
    const uint64_t in_use = SD.usedBytes();
    return in_use > total ? 0 : total - in_use;
}

bool SD_Card::measure() {
    if (_state == _published_state) return true;
    _published_state = _state;
    value(0).from(_state == STATE_MOUNTED ? "mounted"
        : _state == STATE_ERROR ? "error: sd mount" : "unmounted");
    // Always report success: a false return would make the framework replace
    // the measured values with the last published ones.
    return true;
}

#endif // ESP32
