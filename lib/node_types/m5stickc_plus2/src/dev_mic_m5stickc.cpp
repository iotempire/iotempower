// dev_mic_m5stickc.cpp
#include "dev_mic_m5stickc.h"

M5StickC_Mic::M5StickC_Mic(const char* name) : Device(name, 20000) { // 20ms poll rate = 20000 us
    add_subdevice(new Subdevice(F("audio"), false, IOTEMPOWER_MIC_BUFFER_SIZE * sizeof(int16_t))); // subdevice_index=0, with big buffer
}

bool M5StickC_Mic::init() {
    StickCP2.Speaker.end(); // Disable speaker (shares I2S)
    StickCP2.Mic.begin();
    return true;
}

bool M5StickC_Mic::measure() {
    // StickCP2.update(); // might not be necessary as called in power management default platform

    // Check if previous data hasn't been published yet
    if (get_big_buffer_filled (0) > 0) {
        ulog("M5StickC_Mic: Previous buffer not yet published, skipping new recording");
        return false; // Don't record new data until previous buffer is published
    }

    // Use non-blocking record function to fill the buffer
    if (StickCP2.Mic.record(audio_buffer, IOTEMPOWER_MIC_BUFFER_SIZE, 16000)) {
        // Buffer was successfully filled
        // Copy audio data to the big buffer in the subdevice - TODO could be optimized when copying directly to pointer
        big_buffer_from(0, (const uint8_t*)audio_buffer);  // subdevice_index=0, buffer_size=-1 (use full size)
        
        return true;
    }
    
    // Buffer not ready yet, return false and wait for next poll
    return false;
}
