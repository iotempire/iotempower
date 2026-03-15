// dev_mic_m5stickc.h
// Header file for controlling the m5stickc Plus2 Microphone

#ifndef _IOTEMPOWER_M5STICKC_MIC_H_
#define _IOTEMPOWER_M5STICKC_MIC_H_

#include <iotempower.h>

class M5StickC_Mic : public Device {
    private:
        static const int IOTEMPOWER_MIC_BUFFER_SIZE = 1024; // 64ms if sample rate is 16kHz mono
        int16_t audio_buffer[IOTEMPOWER_MIC_BUFFER_SIZE];
        
    protected:
        bool init();
    public:
        M5StickC_Mic(const char* name);
        void start() {
            if (init()) {
                _started = true;
            }
        }
        bool measure();

};

#endif // _IOTEMPOWER_M5STICKC_MIC_H_
