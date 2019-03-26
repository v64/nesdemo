#include <stdio.h>
#include <string.h>

#include "nes.h"
#include "constants.h"

int main(int argc, char *argv[])
{
	#ifdef DEBUG
	printf("Debug mode on.\n");
	#endif
	FILE *rom;
	int ret;

	if (argc == 1) {
		fprintf(stderr, "ERR: No ROM given as input.\n");
		return 1;
	} else {
		if ((rom = fopen(*++argv, "rb")) == NULL) {
			fprintf(stderr, "ERR: Can't open ROM. Is it there?\n");
			return 1;
		} else {
			char nes[4];
			fread(nes, 1, 4, rom);
			if (memcmp(nes, "NES\x1a", sizeof(nes)) != 0) {
				fprintf(stderr, "ERR: File is not an NES ROM.\n");
				return 1;
			}
			ret = start_nes(rom);
		}
	}
	return ret;
}
