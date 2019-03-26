#include "bits.h"
#include "constants.h"
#include "globals.h"
#include "SDL.h"

/* controllers */
unsigned char ctrler1_status = 0x00;
unsigned char ctrler2_status = 0x00;

void controller_input(SDL_KeyboardEvent *key, int key_down)
{
	switch (key->keysym.sym) {
		/* ctrler1 */
		case SDLK_LEFT:
			key_down ? (SET_BIT(ctrler1_status, BUTTON_LEFT)) : (UNSET_BIT(ctrler1_status, BUTTON_LEFT));
			break;
		case SDLK_RIGHT:
			key_down ? (SET_BIT(ctrler1_status, BUTTON_RIGHT)) : (UNSET_BIT(ctrler1_status, BUTTON_RIGHT));
			break;
		case SDLK_UP:
			key_down ? (SET_BIT(ctrler1_status, BUTTON_UP)) : (UNSET_BIT(ctrler1_status, BUTTON_UP));
			break;
		case SDLK_DOWN:
			key_down ? (SET_BIT(ctrler1_status, BUTTON_DOWN)) : (UNSET_BIT(ctrler1_status, BUTTON_DOWN));
			break;
		case SDLK_LSHIFT:
			key_down ? (SET_BIT(ctrler1_status, BUTTON_B)) : (UNSET_BIT(ctrler1_status, BUTTON_B));
			break;
		case SDLK_LALT:
			key_down ? (SET_BIT(ctrler1_status, BUTTON_A)) : (UNSET_BIT(ctrler1_status, BUTTON_A));
			break;
		case SDLK_TAB:
			key_down ? (SET_BIT(ctrler1_status, BUTTON_SELECT)) : (UNSET_BIT(ctrler1_status, BUTTON_SELECT));
			break;
		case SDLK_RETURN:
			key_down ? (SET_BIT(ctrler1_status, BUTTON_START)) : (UNSET_BIT(ctrler1_status, BUTTON_START));
			break;

		/* ctrler2 */
		case SDLK_a:
			key_down ? (SET_BIT(ctrler2_status, BUTTON_LEFT)) : (UNSET_BIT(ctrler2_status, BUTTON_LEFT));
			break;
		case SDLK_d:
			key_down ? (SET_BIT(ctrler2_status, BUTTON_RIGHT)) : (UNSET_BIT(ctrler2_status, BUTTON_RIGHT));
			break;
		case SDLK_w:
			key_down ? (SET_BIT(ctrler2_status, BUTTON_UP)) : (UNSET_BIT(ctrler2_status, BUTTON_UP));
			break;
		case SDLK_s:
			key_down ? (SET_BIT(ctrler2_status, BUTTON_DOWN)) : (UNSET_BIT(ctrler2_status, BUTTON_DOWN));
			break;
		case SDLK_h:
			key_down ? (SET_BIT(ctrler2_status, BUTTON_B)) : (UNSET_BIT(ctrler2_status, BUTTON_B));
			break;
		case SDLK_j:
			key_down ? (SET_BIT(ctrler2_status, BUTTON_A)) : (UNSET_BIT(ctrler2_status, BUTTON_A));
			break;
		case SDLK_k:
			key_down ? (SET_BIT(ctrler2_status, BUTTON_SELECT)) : (UNSET_BIT(ctrler2_status, BUTTON_SELECT));
			break;
		case SDLK_l:
			key_down ? (SET_BIT(ctrler2_status, BUTTON_START)) : (UNSET_BIT(ctrler2_status, BUTTON_START));
			break;
	}
	//fprintf(stderr, "STATUS: %.2x\n", ctrler1_status);
}

unsigned char button_order[] = { BUTTON_A, BUTTON_B, BUTTON_SELECT, BUTTON_START, BUTTON_UP, BUTTON_DOWN, BUTTON_LEFT, BUTTON_RIGHT };

unsigned int ctrler1_counter = 0; // Marks which button we're returning info about.
unsigned int ctrler2_counter = 0;

unsigned char controller_status(unsigned short ctrler)
{
	if (ctrler == CTRLER1) {
		if (ctrler1_counter < 7) {
			ctrler1_counter++;
			return BIT_IS_SET(ctrler1_status, button_order[ctrler1_counter]) ? BUTTON_PRESSED : BUTTON_UNPRESSED;
		} else if ((ctrler1_counter >= 8) && (ctrler1_counter <= 15)) {
			ctrler1_counter++;
			return 0x64;
		} else if (ctrler1_counter == 16) {
			ctrler1_counter++;
			return 0x00;
		} else if (ctrler1_counter == 17) {
			ctrler1_counter++;
			return 0x00;
		} else if (ctrler1_counter == 18) {
			ctrler1_counter++;
			return 0x00;
		} else if (ctrler1_counter == 19) {
			ctrler1_counter++;
			return 0x01;
		} else if ((ctrler1_counter >= 20) && (ctrler1_counter <= 22)) {
			ctrler1_counter++;
			return 0x00;
		} else {
			ctrler1_counter = 0;
			return 0x00;
		}
	} else {
		if (ctrler2_counter < 7) {
			ctrler2_counter++;
			return BIT_IS_SET(ctrler2_status, button_order[ctrler2_counter]) ? BUTTON_PRESSED : BUTTON_UNPRESSED;
		} else if ((ctrler2_counter >= 8) && (ctrler2_counter <= 15)) {
			ctrler2_counter++;
			return 0x64;
		} else if (ctrler2_counter == 16) {
			ctrler2_counter++;
			return 0x01;
		} else if (ctrler2_counter == 17) {
			ctrler2_counter++;
			return 0x00;
		} else if (ctrler2_counter == 18) {
			ctrler2_counter++;
			return 0x00;
		} else if (ctrler2_counter == 19) {
			ctrler2_counter++;
			return 0x00;
		} else if ((ctrler2_counter >= 20) && (ctrler2_counter <= 22)) {
			ctrler2_counter++;
			return 0x01;
		} else {
			ctrler2_counter = 0;
			return 0x00;
		}
	}
}

void reset_controller(unsigned short ctrler)
{
	if (ctrler == CTRLER1) {
		ctrler1_counter = 0;
	} else {
		ctrler2_counter = 0;
	}
}

int event_loop()
{
	static SDL_Event events;
	static int paused = 0;

	paused:
	while (SDL_PollEvent(&events)) {
		switch (events.type) {
			case SDL_QUIT:
				return -1;

			case SDL_KEYDOWN:
				if ((events.key.keysym.sym == SDLK_r) && !paused) {
					printf("\n\nResetting the system...");
					return -2;	
				} else if (events.key.keysym.sym == SDLK_p) {
					if (!paused) {
						printf("\nPausing emulation...");
						paused = 1;
					} else {
						printf("Resuming emulation.\n");
						paused = 0;
					}
				} else if (events.key.keysym.sym == SDLK_ESCAPE) {
					return -1;
				} else {
					controller_input(&events.key, 1);
				}
				break;

			case SDL_KEYUP:
				controller_input(&events.key, 0);
				break;
		}
	}
	if (paused)
		goto paused;
	return 0;
}
