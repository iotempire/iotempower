// audio_status.cpp

#include "audio_status.h"

#ifdef ESP32

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include <toolbox.h>

static QueueHandle_t audio_status_queue = NULL;
// Only used by the player task (set on error, cleared on the next play), so no
// locking is needed. The queue is what crosses over into the main loop.
static volatile bool audio_status_error_flag = false;

void audio_status_start() {
    if (audio_status_queue == NULL) {
        audio_status_queue = xQueueCreate(1, AUDIO_STATUS_MAX_LENGTH);
        if (audio_status_queue == NULL) {
            ulog(F("audio_status: cannot create the status queue."));
        }
    }
}

static void audio_status_post(const char* message) {
    if (audio_status_queue == NULL || message == NULL) return;
    char buffer[AUDIO_STATUS_MAX_LENGTH];
    strncpy(buffer, message, AUDIO_STATUS_MAX_LENGTH - 1);
    buffer[AUDIO_STATUS_MAX_LENGTH - 1] = 0;
    // Queue length is one: the newest message wins.
    xQueueOverwrite(audio_status_queue, buffer);
}

void audio_status_event(const char* message) {
    audio_status_post(message);
}

void audio_status_error(const char* message) {
    audio_status_error_flag = true;
    audio_status_post(message);
}

bool audio_status_has_error() {
    return audio_status_error_flag;
}

void audio_status_clear_error() {
    audio_status_error_flag = false;
}

bool audio_status_take(char* buffer, size_t size) {
    if (audio_status_queue == NULL || buffer == NULL || size == 0) return false;
    char message[AUDIO_STATUS_MAX_LENGTH];
    if (xQueueReceive(audio_status_queue, message, 0) != pdTRUE) return false;
    strncpy(buffer, message, size - 1);
    buffer[size - 1] = 0;
    return true;
}

#endif // ESP32
