#!/bin/sh

# brew install sdl

gcc -O3 -Wall \
	-lSDL -I/usr/local/Cellar/sdl/1.2.15_1/include/SDL/ -I./include \
	nes.c memory.c cpu.c ppu.c \
	main.c video.c input.c \
	-o nesdemo
