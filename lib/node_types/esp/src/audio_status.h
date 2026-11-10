// audio_status.h
// Status events from the audio sources and the player task to the main loop,
// where the player publishes them as <device>/status (idle, playing <file>,
// error: <reason>).
//
// The player task must not touch the framework's value strings directly, so
// the messages go through a one entry queue: the newest message wins and a
// burst collapses to its last entry. The player drains it in measure(), which
// runs in the main loop.

#ifndef _AUDIO_STATUS_H_
#define _AUDIO_STATUS_H_

#include <stdint.h>
#include <stddef.h>

#ifdef ESP32

// Longest status message including the terminating zero. Matches the
// framework string limit, so a message is never truncated twice.
#define AUDIO_STATUS_MAX_LENGTH 128

// Called once from the player's start(). Events before that are ignored.
void audio_status_start();

// Normal state: "idle", "playing <file>".
void audio_status_event(const char* message);

// Error state: posts the message and sets the sticky error flag. The player
// uses the flag to suppress its trailing "idle", so an error that ends the
// stream stays visible.
void audio_status_error(const char* message);

bool audio_status_has_error();
void audio_status_clear_error();

// Takes the pending message, if any. Called from measure() in the main loop.
bool audio_status_take(char* buffer, size_t size);

#endif // ESP32

#endif // _AUDIO_STATUS_H_
