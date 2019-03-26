#include <stdio.h>
#include "constants.h"
#include "bits.h"
#include "globals.h"
#include "registers.h"

#include "input.h"

/* create CPU/PPU memory */
/* yeah it's the whole thing, will do proper paging later! */
unsigned char cpu_memory[CPU_MEMORY_SIZE];
unsigned char ppu_memory[PPU_MEMORY_SIZE];

unsigned char low_byte;
unsigned char high_byte;
unsigned short temp_addr;
unsigned char vram_byte = 0x64;
int dma_ready = 0;
int ctrler_reset = 0;

void load_roms_into_memory(FILE *rom, unsigned char prg_rom_num, unsigned char chr_rom_num)
{
	fseek(rom, 16, SEEK_SET);

	/* copy ROM into the proper place in cpu memory */
	fread((cpu_memory+LOWER_PRG_ROM), 1, PRG_ROM_SIZE, rom);
	if (prg_rom_num == 1)
		fseek(rom, 16, SEEK_SET);
	fread((cpu_memory+UPPER_PRG_ROM), 1, PRG_ROM_SIZE, rom);

	/* copy character ROM into ppu memory, dunno if this is right yet */
	if (chr_rom_num != 0)
		fread((ppu_memory+CHR_ROM1), 1, CHR_ROM_SIZE, rom);
}

unsigned char read_cpu_memory(unsigned short addr, int actual_read)
{
	addr &= 0xFFFF; // Unnecessary, but accurate, overflow checker

	if ((addr >= RAM_MIRROR_START) && (addr <= RAM_MIRROR_END)) { /* Zero page, stack, RAM mirror */
		addr &= (RAM_MIRROR_START - 1);
	} else if ((addr >= REG_MIRROR_START) && (addr <= REG_MIRROR_END)) { /* Register mirror */
		addr &= (REG_MIRROR_START - 1);
	}

	if ((addr >= PPU_CTRL_REG1) && (addr <= PPU_MEM_DATA) && actual_read) {
		unsigned char return_val;
		switch (addr) {
			case PPU_STATUS:
				return_val = cpu_memory[PPU_STATUS];
				UNSET_BIT(cpu_memory[PPU_STATUS], PPU_IN_VBLANK);
				return return_val;

			case PPU_SPRITE_MEM_DATA:
				return_val = ppu_memory[cpu_memory[PPU_SPRITE_MEM_ADDR]];
				cpu_memory[PPU_SPRITE_MEM_ADDR] += 1;
				return return_val;

			case PPU_MEM_DATA:
				temp_addr = BYTE_ADDR(low_byte, high_byte);
				temp_addr &= 0x3FFF;
				if ((temp_addr >= PPU_TABLE_MIRROR_START) && (temp_addr <= PPU_TABLE_MIRROR_END)) {
					temp_addr &= (PPU_TABLE_MIRROR_START - 1);
				} else if ((temp_addr >= PPU_PALETTE_MIRROR_START) && (temp_addr <= PPU_PALETTE_MIRROR_END)) {
					temp_addr &= (PPU_PALETTE_MIRROR_START - 1);
				}

				if (temp_addr <= 0x3EFF) {
					return_val = vram_byte;
					vram_byte = ppu_memory[temp_addr];
					temp_addr += (BIT_IS_SET(cpu_memory[PPU_CTRL_REG1], PPU_VERTICAL_WRITE)) ? 32 : 1;
					low_byte = GET_LOW_BYTE(temp_addr);
					high_byte = GET_HIGH_BYTE(temp_addr);
					return return_val;
				} else {
					return_val = ppu_memory[temp_addr];
					temp_addr += (BIT_IS_SET(cpu_memory[PPU_CTRL_REG1], PPU_VERTICAL_WRITE)) ? 32 : 1;
					low_byte = GET_LOW_BYTE(temp_addr);
					high_byte = GET_HIGH_BYTE(temp_addr);
					return return_val;
				}
		}
	} else if ((addr == CTRLER1) || (addr == CTRLER2)) {
		controller_status(addr);
	}

	return cpu_memory[addr];
}

void write_cpu_memory(unsigned short addr, unsigned char data)
{
	addr &= 0xFFFF; // Unnecessary, but accurate, overflow checker

	/* I've been told the actual NES doesn't write-protect anything */
	if (addr >= LOWER_PRG_ROM) {
		#ifdef DEBUG
		fprintf(stderr, "WARN: Attempted to write to PRG-ROM.\n");
		#endif
		return;
	}

	if ((addr >= RAM_MIRROR_START) && (addr <= RAM_MIRROR_END)) { /* Zero page, stack, RAM mirror */
		addr &= (RAM_MIRROR_START - 1);
	} else if ((addr >= REG_MIRROR_START) && (addr <= REG_MIRROR_END)) { /* Register mirror */
		addr &= (REG_MIRROR_START - 1);
	}

	if ((addr >= PPU_CTRL_REG1) && (addr <= PPU_MEM_DATA)) {
		switch (addr) {
			case PPU_MEM_ADDR:
				if (dma_ready == 0) {
					high_byte = data;
					cpu_memory[PPU_MEM_ADDR] = data;
					dma_ready++;
				} else {
					low_byte = data;
					cpu_memory[PPU_MEM_ADDR] = data;
					dma_ready--;
				}
				return;

			case PPU_MEM_DATA:
				temp_addr = BYTE_ADDR(low_byte, high_byte);
				temp_addr &= 0x3FFF;
				if ((temp_addr >= PPU_TABLE_MIRROR_START) && (temp_addr <= PPU_TABLE_MIRROR_END)) {
					temp_addr &= (PPU_TABLE_MIRROR_START - 1);
				} else if ((temp_addr >= PPU_PALETTE_MIRROR_START) && (temp_addr <= PPU_PALETTE_MIRROR_END)) {
					temp_addr &= (PPU_PALETTE_MIRROR_START - 1);
				}
				ppu_memory[temp_addr] = data;
				temp_addr += (BIT_IS_SET(cpu_memory[PPU_CTRL_REG1], PPU_VERTICAL_WRITE)) ? 32 : 1;
				low_byte = GET_LOW_BYTE(temp_addr);
				high_byte = GET_HIGH_BYTE(temp_addr);
				return;
		}
	} else if ((addr == CTRLER1) || (addr == CTRLER2)) {
		if (data == 0x01) {
			ctrler_reset++;
		} else if ((data == 0x00) && ctrler_reset) {
			reset_controller(addr);
			ctrler_reset--;
		}
	}

	cpu_memory[addr] = data;
}

void push_cpu_stack(unsigned char data)
{
	cpu_memory[BYTE_ADDR(cpu_regs.reg_S, 0x01)] = data;
	(cpu_regs.reg_S)--;
}

unsigned char pull_cpu_stack()
{
	(cpu_regs.reg_S)++;
	unsigned char return_val = cpu_memory[BYTE_ADDR(cpu_regs.reg_S, 0x01)];
	return return_val;
}
