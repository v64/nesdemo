#include "SDL.h"

void controller_input(SDL_KeyboardEvent *, int);
unsigned char controller_status(unsigned short);
void reset_controller(unsigned short ctrler);
int event_loop();
