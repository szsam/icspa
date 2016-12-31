#include "hal.h"

#define NR_KEYS 18

enum {KEY_STATE_EMPTY, KEY_STATE_WAIT_RELEASE, KEY_STATE_RELEASE, KEY_STATE_PRESS};

/* Only the following keys are used in NEMU-PAL. */
static const int keycode_array[] = {
	K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ESCAPE,
	K_RETURN, K_SPACE, K_PAGEUP, K_PAGEDOWN, K_r,
	K_a, K_d, K_e, K_w, K_q,
	K_s, K_f, K_p
};

static int key_state[NR_KEYS];

// set when the last scancode received is 0xf0, clear otherwise
static bool last_data_is_0xf0 = false;

void
keyboard_event(void) {
	/* TODO: Fetch the scancode and update the key states. */
	// assert(0);
	uint8_t key_code = in_byte(0x60);
	int i = 0;
	while (i < NR_KEYS && keycode_array[i] != key_code) {
		i++;
	}

	if (!last_data_is_0xf0) {
		if (i < NR_KEYS)
			key_state[i] = KEY_STATE_PRESS;
		else {
			assert(key_code == 0xf0);
			last_data_is_0xf0 = true;
		}
	}
	else {
		assert(i < NR_KEYS);
		assert(key_state[i] == KEY_STATE_PRESS || key_state[i] == KEY_STATE_WAIT_RELEASE);
		key_state[i] = KEY_STATE_RELEASE;
		last_data_is_0xf0 = false;
	}
			
}

/*
static inline int
get_keycode(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return keycode_array[index];
}

static inline int
query_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return key_state[index];
}

static inline void
release_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_WAIT_RELEASE;
}

static inline void
clear_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_EMPTY;
}
*/

bool 
process_keys(void (*key_press_callback)(int), void (*key_release_callback)(int)) {
	cli();
	/* TODO: Traverse the key states. Find a key just pressed or released.
	 * If a pressed key is found, call `key_press_callback' with the keycode.
	 * If a released key is found, call `key_release_callback' with the keycode.
	 * If any such key is found, the function return true.
	 * If no such key is found, the function return false.
	 * Remember to enable interrupts before returning from the function.
	 */

	for (int i = 0; i < NR_KEYS; i++) {
		int state = key_state[i];
		if (state == KEY_STATE_PRESS || state == KEY_STATE_RELEASE ) {
			if (state == KEY_STATE_PRESS) {
				key_press_callback(keycode_array[i]);
				key_state[i] = KEY_STATE_WAIT_RELEASE;
			}
			else {
				key_release_callback(keycode_array[i]);
				key_state[i] = KEY_STATE_EMPTY;
			}
			sti();
			return true;
		}
	}
	sti();
	return false;
}
