#include <stdio.h>

#include "bits.h"
#include "constants.h"
#include "cpu.h"
#include "globals.h"
#include "memory.h"
#include "ppu.h"
#include "registers.h"

#include "input.h"
#include "video.h"

static inline void reset_nes(int *current_scanline, int *cycle_count, const int CYCLES_PER_SCANLINE)
{
	*current_scanline = 0;
	*cycle_count = CYCLES_PER_SCANLINE;

	/* This is pretty much all that's guaranteed on boot.
	   I hope you initialized memory and the stack pointer yourself. */
	SET_BIT(cpu_flags, CPU_FLAG_I);
	cpu_regs.reg_P = BYTE_ADDR(read_cpu_memory(RESET_INT_LOW, 1), read_cpu_memory(RESET_INT_HIGH, 1));

	/* TODO: set initial states of registers */
}

int start_nes(FILE *rom)
{
	int ret;

	/* Read various info about the ROM from the header */
	fseek(rom, 4, SEEK_SET);
	unsigned char prg_rom_num = getc(rom);

	if (prg_rom_num > 2) {
		fprintf(stderr, "ERR: Memory mapping is hard, I'll implement it later.\n");
		ret = 1;
		goto emu_exit;
	}

	unsigned char chr_rom_num = getc(rom);
	unsigned char data1 = getc(rom); /* important VRAM info that needs to be set */
	unsigned char data2 = getc(rom);
	unsigned char kb8_ram_num = getc(rom);
	unsigned char ntsc_rom = BIT_IS_SET(getc(rom), 0x01) ? 0 : 1; /* bit on signifies PAL, I know I did this backwards */

	#ifdef DEBUG
	printf("PRG_ROMs: %d\nCHR_ROMs: %d\nDATA1: %d\nDATA2: %d\nKB8_RAMs: %d\nNTSC_ROM: %d\n\n",
			prg_rom_num, chr_rom_num, data1, data2, kb8_ram_num, ntsc_rom);
	#endif

	load_roms_into_memory(rom, prg_rom_num, chr_rom_num);
	fclose(rom);

	/* Random misc vars needed to do CPU things */
	const int CYCLES_PER_SCANLINE = ntsc_rom ? 114 : 107;
	const int SCANLINE_NUM = 242;
	const int VBLANK_NUM = ntsc_rom ? 20 : 70;
	const int SCR_WIDTH = 256;
	//const int SCR_HEIGHT = ntsc_rom ? 224 : 240;
	const int SCR_HEIGHT = 240;
	int cycle_count = CYCLES_PER_SCANLINE;
	int current_scanline = 0;

	ret = video_init(SCR_WIDTH, SCR_HEIGHT);
	if (ret != 0)
		goto emu_exit;

	do_reset:
	printf("Starting emulation!\n\n");

	reset_nes(&current_scanline, &cycle_count, CYCLES_PER_SCANLINE);

	for (;;) {
		cycle_count = next_cpu_instr(cycle_count);

		if (cycle_count <= 0) {
			cycle_count += CYCLES_PER_SCANLINE;

			ret = event_loop();
			if (ret == -1) {
				ret = 0;
				goto emu_exit;
			} else if (ret == -2) {
				goto do_reset;
			} else if (ret > 0) {
				goto emu_exit;
			}

			if ((current_scanline >= 0) && (current_scanline <= SCR_HEIGHT-1)) {
				draw_scanline(current_scanline);
				current_scanline++;
				// scanlines 240-242 take time entering v-blank
			} else if (current_scanline == SCANLINE_NUM + 1) {
				// begin vblank, interrupt if necessary
				// VBlank should occur 60 times a second in NTSC, 50 in PAL
				SET_BIT(cpu_memory[PPU_STATUS], PPU_IN_VBLANK);
				if (BIT_IS_SET(read_cpu_memory(PPU_CTRL_REG1, 1), PPU_VBLANK_INT)) {
					cycle_count -= 7;
					push_cpu_stack(GET_HIGH_BYTE(cpu_regs.reg_P));
					push_cpu_stack(GET_LOW_BYTE(cpu_regs.reg_P));
					push_cpu_stack(cpu_flags);
					SET_BIT(cpu_flags, CPU_FLAG_I);
					cpu_regs.reg_P = BYTE_ADDR(read_cpu_memory(NMI_INT_LOW, 1), read_cpu_memory(NMI_INT_HIGH, 1));
				}
				current_scanline++;
			} else if (current_scanline == SCANLINE_NUM + 1 + VBLANK_NUM) {
				// vblank over, reset the things
				UNSET_BIT(cpu_memory[PPU_STATUS], PPU_HIT_FLAG | PPU_IN_VBLANK);
				current_scanline = 0;
			} else {
				current_scanline++;
			}
		}
	}

	emu_exit:
	return ret;
}
