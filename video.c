#include "SDL.h"
#include "constants.h"

SDL_Surface *screen;

void draw_pixel(int x, int y, int color)
{
	unsigned int *ptr = (unsigned int *)screen->pixels;
	int lineoffset = y * (screen->pitch / 4);
	ptr[lineoffset + x] = color;
}

int video_init(const int SCR_WIDTH, const int SCR_HEIGHT)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "ERR: Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);
	SDL_WM_SetCaption("nesdemo", "nesdemo");
	if ((screen = SDL_SetVideoMode(SCR_WIDTH, SCR_HEIGHT, 32, SDL_HWSURFACE)) == NULL) {
		fprintf(stderr, "ERR: Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	return 0;
}

void drawing_init()
{
	if (SDL_MUSTLOCK(screen)) {
		if (SDL_LockSurface(screen) < 0)
			return;
	}
}

void drawing_close(int current_scanline)
{
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, current_scanline, 256, 1);
}
