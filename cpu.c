/* The big ol' 6502 CPU emulator */

#include <stdio.h>

#include "bits.h"
#include "constants.h"
#include "globals.h"
#include "memory.h"
#include "registers.h"

/* set up CPU register and flag variables */
struct cpu_regs_list cpu_regs;
unsigned char cpu_flags;

int next_cpu_instr(int cycle_count)
{
	#ifdef DEBUG
	printf("$%.4x:", cpu_regs.reg_P);
	#endif

	switch (read_cpu_memory(cpu_regs.reg_P, 1)) {
		/* adc - add with carry */
		case 0x69:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					int tmp_int = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("69 %.2x     |  adc #$%.2x                  ", tmp_int, tmp_int);
					#endif

					// NES lacks decimal mode, so I won't bother with it here for now.
					if (BIT_IS_SET(cpu_flags, CPU_FLAG_C))
						tmp_int = cpu_regs.reg_A + tmp_int + 1;
					else
						tmp_int = cpu_regs.reg_A + tmp_int;

					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N) != BIT_IS_SET(tmp_int, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_V) : UNSET_BIT(cpu_flags, CPU_FLAG_V);
					(tmp_int > 255) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(tmp_int == 0)  ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					BIT_IS_SET(tmp_int, CPU_FLAG_N) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					cpu_regs.reg_A = tmp_int & 0xFF;
					break;
		}

		case 0x65:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					int tmp_int = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("65 %.2x     |  adc $%.2x = #$%.2x            ", low_byte, low_byte, tmp_int);
					#endif

					// NES lacks decimal mode, so I won't bother with it here for now.
					if (BIT_IS_SET(cpu_flags, CPU_FLAG_C))
						tmp_int = cpu_regs.reg_A + tmp_int + 1;
					else
						tmp_int = cpu_regs.reg_A + tmp_int;

					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N) != BIT_IS_SET(tmp_int, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_V) : UNSET_BIT(cpu_flags, CPU_FLAG_V);
					(tmp_int > 255) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(tmp_int == 0)  ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					BIT_IS_SET(tmp_int, CPU_FLAG_N) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					cpu_regs.reg_A = tmp_int & 0xFF;
					break;
		}

		case 0x6D:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					int tmp_int = read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 1);
					#ifdef DEBUG
					printf("6d %.2x %.2x  |  adc $%.2x%.2x = $%.4x = #$%.2x  ", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte), tmp_int);
					#endif

					// NES lacks decimal mode, so I won't bother with it here for now.
					if (BIT_IS_SET(cpu_flags, CPU_FLAG_C))
						tmp_int = cpu_regs.reg_A + tmp_int + 1;
					else
						tmp_int = cpu_regs.reg_A + tmp_int;

					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N) != BIT_IS_SET(tmp_int, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_V) : UNSET_BIT(cpu_flags, CPU_FLAG_V);
					(tmp_int > 255) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(tmp_int == 0)  ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					BIT_IS_SET(tmp_int, CPU_FLAG_N) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					cpu_regs.reg_A = tmp_int & 0xFF;
					break;
		}

		case 0x79:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					int tmp_int = read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, 1);
					#ifdef DEBUG
					printf("79 %.2x %.2x  |  adc $%.2x%.2x,Y = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, tmp_int);
					#endif

					if (GET_HIGH_BYTE(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y) != high_byte)
						cycle_count -= 1;

					// NES lacks decimal mode, so I won't bother with it here for now.
					if (BIT_IS_SET(cpu_flags, CPU_FLAG_C))
						tmp_int = cpu_regs.reg_A + tmp_int + 1;
					else
						tmp_int = cpu_regs.reg_A + tmp_int;

					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N) != BIT_IS_SET(tmp_int, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_V) : UNSET_BIT(cpu_flags, CPU_FLAG_V);
					(tmp_int > 255) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(tmp_int == 0)  ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					BIT_IS_SET(tmp_int, CPU_FLAG_N) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					cpu_regs.reg_A = tmp_int & 0xFF;
					break;
		}

		/* and - bitwise and with accumulator */
		case 0x29:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("29 %.2x     |  and #$%.2x                  ", low_byte, low_byte);
					#endif

					cpu_regs.reg_A &= low_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x25:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 1);
					#ifdef DEBUG
					printf("25 %.2x %.2x  |  and $%.2x%.2x = #$%.2x          ", low_byte, high_byte, high_byte, low_byte, tmp_byte);
					#endif

					cpu_regs.reg_A &= tmp_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x35:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X, 0);
					#ifdef DEBUG
					printf("35 %.2x     |  and $%.2x,X = $%.4x = #$%.2x  ", low_byte, low_byte,
																			BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X, tmp_byte);
					#endif

					cpu_regs.reg_A &= tmp_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x3D:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, 0);
					#ifdef DEBUG
					printf("3d %.2x %.2x  |  and $%.2x%.2x,X = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, tmp_byte);
					#endif

					if (GET_HIGH_BYTE(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X) != high_byte)
						cycle_count -= 1;

					cpu_regs.reg_A &= tmp_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* asl - arithmetic shift left */
		case 0x0A:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("0a        |  asl                       ");
					#endif

					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					cpu_regs.reg_A = (cpu_regs.reg_A << 1) & ~0x01; // Unset first bit of cpu_regs.reg_A << 1
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x06:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 5;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("06 %.2x     |  asl $%.2x = #$%.2x            ", low_byte, low_byte, tmp_byte);
					#endif

					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					tmp_byte = (tmp_byte << 1) & ~0x01; // Unset first bit of cpu_regs.reg_A << 1
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					write_cpu_memory(BYTE_ADDR(low_byte, 0x00), tmp_byte);
					break;
		}

		/* bit - test bits */
		case 0x24:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("24 %.2x     |  bit $%.2x = #$%.2x            ", low_byte, low_byte, tmp_byte);
					#endif
					tmp_byte = cpu_regs.reg_A & tmp_byte;
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_V)) ? SET_BIT(cpu_flags, CPU_FLAG_V) : UNSET_BIT(cpu_flags, CPU_FLAG_V);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x2C:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 0);
					#ifdef DEBUG
					printf("2c %.2x %.2x  |  bit $%.2x%.2x = #$%.2x          ", low_byte, high_byte, high_byte, low_byte, tmp_byte);
					#endif
					tmp_byte = cpu_regs.reg_A & tmp_byte;
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_V)) ? SET_BIT(cpu_flags, CPU_FLAG_V) : UNSET_BIT(cpu_flags, CPU_FLAG_V);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* branch instructions */
		case 0x10:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					if (BIT_IS_SET(cpu_flags, CPU_FLAG_N)) {
						#ifdef DEBUG
						unsigned char tmp_byte = read_cpu_memory(cpu_regs.reg_P - 1, 0);
						printf("10 %.2x     |  bpl $%.2x = $%.4x           ",
								tmp_byte, tmp_byte, cpu_regs.reg_P + (char)tmp_byte);
						#endif
						break;
					}

					cycle_count -= 1;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);

					/* if (low_byte <= 127)
						tmp_addr = cpu_regs.reg_P + low_byte;
					else
						tmp_addr = cpu_regs.reg_P - (256 - low_byte); */

					unsigned short tmp_addr = cpu_regs.reg_P + (char)low_byte;

					if (GET_HIGH_BYTE(tmp_addr) != GET_HIGH_BYTE(cpu_regs.reg_P))
						cycle_count--;

					cpu_regs.reg_P = tmp_addr;

					#ifdef DEBUG
					printf("10 %.2x     |  bpl $%.2x = $%.4x           ", low_byte, low_byte, cpu_regs.reg_P);
					#endif

					break;
		}

		case 0x30:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					if (!BIT_IS_SET(cpu_flags, CPU_FLAG_N)) {
						#ifdef DEBUG
						unsigned char tmp_byte = read_cpu_memory(cpu_regs.reg_P - 1, 0);
						printf("30 %.2x     |  bmi $%.2x = $%.4x           ",
								tmp_byte, tmp_byte, cpu_regs.reg_P + (char)tmp_byte);
						#endif
						break;
					}

					cycle_count -= 1;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);

					/* if (low_byte <= 127)
						tmp_addr = cpu_regs.reg_P + low_byte;
					else
						tmp_addr = cpu_regs.reg_P - (256 - low_byte); */

					unsigned short tmp_addr = cpu_regs.reg_P + (char)low_byte;

					if (GET_HIGH_BYTE(tmp_addr) != GET_HIGH_BYTE(cpu_regs.reg_P))
						cycle_count--;

					cpu_regs.reg_P = tmp_addr;

					#ifdef DEBUG
					printf("30 %.2x     |  bmi $%.2x = $%.4x           ", low_byte, low_byte, cpu_regs.reg_P);
					#endif

					break;
		}

		case 0x90:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					if (BIT_IS_SET(cpu_flags, CPU_FLAG_C)) {
						#ifdef DEBUG
						unsigned char tmp_byte = read_cpu_memory(cpu_regs.reg_P - 1, 0);
						printf("90 %.2x     |  bcc $%.2x = $%.4x           ",
								tmp_byte, tmp_byte, cpu_regs.reg_P + (char)tmp_byte);
						#endif
						break;
					}

					cycle_count -= 1;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);

					unsigned short tmp_addr = cpu_regs.reg_P + (char)low_byte;

					if (GET_HIGH_BYTE(tmp_addr) != GET_HIGH_BYTE(cpu_regs.reg_P))
						cycle_count--;

					cpu_regs.reg_P = tmp_addr;

					#ifdef DEBUG
					printf("90 %.2x     |  bcc $%.2x = $%.4x           ", low_byte, low_byte, cpu_regs.reg_P);
					#endif

					break;
		}

		case 0xB0:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					if (!BIT_IS_SET(cpu_flags, CPU_FLAG_C)) {
						#ifdef DEBUG
						unsigned char tmp_byte = read_cpu_memory(cpu_regs.reg_P - 1, 0);
						printf("b0 %.2x     |  bcs $%.2x = $%.4x           ",
								tmp_byte, tmp_byte, cpu_regs.reg_P + (char)tmp_byte);
						#endif
						break;
					}

					cycle_count -= 1;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);

					unsigned short tmp_addr = cpu_regs.reg_P + (char)low_byte;

					if (GET_HIGH_BYTE(tmp_addr) != GET_HIGH_BYTE(cpu_regs.reg_P))
						cycle_count--;

					cpu_regs.reg_P = tmp_addr;

					#ifdef DEBUG
					printf("b0 %.2x     |  bcs $%.2x = $%.4x           ", low_byte, low_byte, cpu_regs.reg_P);
					#endif

					break;
		}

		case 0xD0:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					if (BIT_IS_SET(cpu_flags, CPU_FLAG_Z)) {
						#ifdef DEBUG
						unsigned char tmp_byte = read_cpu_memory(cpu_regs.reg_P - 1, 0);
						printf("d0 %.2x     |  bne $%.2x = $%.4x           ",
								tmp_byte, tmp_byte, cpu_regs.reg_P + (char)tmp_byte);
						#endif
						break;
					}

					cycle_count -= 1;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);

					unsigned short tmp_addr = cpu_regs.reg_P + (char)low_byte;

					if (GET_HIGH_BYTE(tmp_addr) != GET_HIGH_BYTE(cpu_regs.reg_P))
						cycle_count--;

					cpu_regs.reg_P = tmp_addr;

					#ifdef DEBUG
					printf("d0 %.2x     |  bne $%.2x = $%.4x           ", low_byte, low_byte, cpu_regs.reg_P);
					#endif

					break;
		}

		case 0xF0:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					if (!BIT_IS_SET(cpu_flags, CPU_FLAG_Z)) {
						#ifdef DEBUG
						unsigned char tmp_byte = read_cpu_memory(cpu_regs.reg_P - 1, 0);
						printf("f0 %.2x     |  beq $%.2x = $%.4x           ",
								tmp_byte, tmp_byte, cpu_regs.reg_P + (char)tmp_byte);
						#endif
						break;
					}

					cycle_count -= 1;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);

					unsigned short tmp_addr = cpu_regs.reg_P + (char)low_byte;

					if (GET_HIGH_BYTE(tmp_addr) != GET_HIGH_BYTE(cpu_regs.reg_P))
						cycle_count--;

					cpu_regs.reg_P = tmp_addr;

					#ifdef DEBUG
					printf("f0 %.2x     |  beq $%.2x = $%.4x           ", low_byte, low_byte, cpu_regs.reg_P);
					#endif

					break;
		}

		/* brk - break */

		/* cmp - compare accumulator */
		case 0xC9:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("c9 %.2x     |  cmp #$%.2x                  ", low_byte, low_byte);
					#endif

					(cpu_regs.reg_A >= low_byte) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(cpu_regs.reg_A == low_byte) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(cpu_regs.reg_A <  low_byte) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xC5:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("c5 %.2x     |  cmp $%.2x = #$%.2x            ", low_byte, low_byte, tmp_byte);
					#endif

					(cpu_regs.reg_A >= tmp_byte) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(cpu_regs.reg_A == tmp_byte) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(cpu_regs.reg_A <  tmp_byte) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xCD:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 1);
					#ifdef DEBUG
					printf("cd %.2x %.2x  |  cmp $%.2x%.2x = #$%.2x          ", low_byte, high_byte, high_byte, low_byte, tmp_byte);
					#endif

					(cpu_regs.reg_A >= tmp_byte) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(cpu_regs.reg_A == tmp_byte) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(cpu_regs.reg_A <  tmp_byte) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xD9:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, 1);
					#ifdef DEBUG
					printf("d9 %.2x %.2x  |  cmp $%.2x%.2x,Y = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				 BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, tmp_byte);
					#endif

					if (GET_HIGH_BYTE(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y) != GET_HIGH_BYTE(cpu_regs.reg_P))
						cycle_count--;

					(cpu_regs.reg_A >= tmp_byte) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(cpu_regs.reg_A == tmp_byte) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(cpu_regs.reg_A <  tmp_byte) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* cpx - compare X register */
		case 0xE0:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("e0 %.2x     |  cpx #$%.2x                  ", low_byte, low_byte);
					#endif

					(cpu_regs.reg_X >= low_byte) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(cpu_regs.reg_X == low_byte) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_Y - low_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* cpy - compare Y register */
		case 0xC0:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("c0 %.2x     |  cpy #$%.2x                  ", low_byte, low_byte);
					#endif

					(cpu_regs.reg_Y >= low_byte) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(cpu_regs.reg_Y == low_byte) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_Y - low_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* dec - decrement memory */
		case 0xC6:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 5;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("c6 %.2x     |  dec $%.4x = #$%.2x          ", low_byte, low_byte, tmp_byte);
					#endif
					tmp_byte--;
					write_cpu_memory(BYTE_ADDR(low_byte, 0x00), tmp_byte);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xD6:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 6;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X, 1);
					#ifdef DEBUG
					printf("d6 %.2x  |  dec $%.2x,X = $%.4x = #$%.2x", low_byte, low_byte,
																	   BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X, tmp_byte);
					#endif
					tmp_byte--;
					write_cpu_memory(BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X, tmp_byte);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xCE:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 6;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 1);
					#ifdef DEBUG
					printf("ce %.2x %.2x  |  dec $%.2x%.2x = #$%.2x          ", low_byte, high_byte, high_byte, low_byte, tmp_byte);
					#endif
					tmp_byte--;
					write_cpu_memory(BYTE_ADDR(low_byte, high_byte), tmp_byte);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xDE:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 7;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, 1);
					#ifdef DEBUG
					printf("de %.2x %.2x  |  dec $%.2x%.2x,X = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, tmp_byte);
					#endif
					tmp_byte--;
					write_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, tmp_byte);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* eor - bitwise exclusive or */
		case 0x49:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("49 %.2x     |  eor #$%.2x                  ", low_byte, low_byte);
					#endif

					cpu_regs.reg_A ^= low_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x45:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("45 %.2x     |  eor $%.2x = #$%.2x            ", low_byte, low_byte, tmp_byte);
					#endif

					cpu_regs.reg_A ^= tmp_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* processor status flag instructions */
		case 0x18:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("18        |  clc                       ");
					#endif

					UNSET_BIT(cpu_flags, CPU_FLAG_C);
					break;
		}

		case 0x38:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("38        |  sec                       ");
					#endif

					SET_BIT(cpu_flags, CPU_FLAG_C);
					break;
		}

		case 0x78:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("78        |  sei                       ");
					#endif

					SET_BIT(cpu_flags, CPU_FLAG_I);
					break;
		}

		case 0xD8:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					#ifdef DEBUG
					printf("d8        |  cld                       ");
					#endif

					UNSET_BIT(cpu_flags, CPU_FLAG_D);
					break;
		}

		/* inc - increment memory */
		case 0xE6:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 5;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("e6 %.2x     |  inc $%.2x = #$%.2x            ", low_byte, low_byte, tmp_byte);
					#endif
					tmp_byte++;
					write_cpu_memory(BYTE_ADDR(low_byte, 0x00), tmp_byte);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xEE:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 6;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 1);
					#ifdef DEBUG
					printf("ee %.2x %.2x  |  inc $%.2x%.2x                 ", low_byte, high_byte, high_byte, low_byte);
					#endif
					tmp_byte++;
					write_cpu_memory(BYTE_ADDR(low_byte, high_byte), tmp_byte);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* jmp - jump */
		case 0x4C:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("4c %.2x %.2x  |  jmp $%.2x%.2x                 ", low_byte, high_byte, high_byte, low_byte);
					#endif

					cpu_regs.reg_P = BYTE_ADDR(low_byte, high_byte);
					break;
		}

		case 0x6C:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 5;

					unsigned char low_byte_indir = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte_indir = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned short tmp_addr = BYTE_ADDR(low_byte_indir, high_byte_indir);
					unsigned char low_byte = read_cpu_memory(tmp_addr, 1);
					unsigned char high_byte = read_cpu_memory(tmp_addr + 1, 1);
					#ifdef DEBUG
					printf("6c %.2x %.2x  |  jmp ($%.4x) = $%2.x%2.x       ", low_byte_indir, high_byte_indir, tmp_addr, high_byte, low_byte);
					#endif

					cpu_regs.reg_P = BYTE_ADDR(low_byte, high_byte);
					break;
		}

		/* jsr - jump to subroutine */
		case 0x20:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 6;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("20 %.2x %.2x  |  jsr $%.2x%.2x                 ", low_byte, high_byte, high_byte, low_byte);
					#endif

					push_cpu_stack(GET_HIGH_BYTE(cpu_regs.reg_P - 1));
					push_cpu_stack(GET_LOW_BYTE(cpu_regs.reg_P - 1));
					cpu_regs.reg_P = BYTE_ADDR(low_byte, high_byte);
					break;
		}

		/* lda - load accumulator */
		case 0xA9:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					cpu_regs.reg_A = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("a9 %.2x     |  lda #$%.2x                  ", cpu_regs.reg_A, cpu_regs.reg_A);
					#endif

					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xA5:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("a5 %.2x     |  lda $%.2x = #$%.2x            ", low_byte, low_byte, tmp_byte);
					#endif

					cpu_regs.reg_A = tmp_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xB5:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X, 1);
					#ifdef DEBUG
					printf("b5 %.2x     |  lda $%.2x,X = $%.4x = #$%.2x  ", low_byte, low_byte,
																	   		BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X, tmp_byte);
					#endif

					cpu_regs.reg_A = tmp_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xAD:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 1);
					#ifdef DEBUG
					printf("ad %.2x %.2x  |  lda $%.2x%.2x = #$%.2x          ", low_byte, high_byte, high_byte, low_byte, tmp_byte);
					#endif

					cpu_regs.reg_A = tmp_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xBD:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, 1);
					#ifdef DEBUG
					printf("bd %.2x %.2x  |  lda $%.2x%.2x,X = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X,
																				tmp_byte);
					#endif

					if (GET_HIGH_BYTE(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X) != high_byte)
						cycle_count -= 1;

					cpu_regs.reg_A = tmp_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xB9:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, 1);
					#ifdef DEBUG
					printf("b9 %.2x %.2x  |  lda $%.2x%.2x,Y = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, tmp_byte);
					#endif

					if (GET_HIGH_BYTE(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y) != high_byte)
						cycle_count -= 1;

					cpu_regs.reg_A = tmp_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xB1:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 5;

					unsigned char low_byte_indir = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char high_byte_indir = low_byte_indir + 1;
					unsigned char low_byte = read_cpu_memory(BYTE_ADDR(low_byte_indir, 0x00), 1);
					unsigned char high_byte = read_cpu_memory(BYTE_ADDR(high_byte_indir, 0x00), 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, 1);
					#ifdef DEBUG
					printf("b1 %.2x     |  lda ($%.2x),Y = $%.4x = #$%.2x", low_byte_indir, low_byte_indir, BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y,
																			tmp_byte);
					#endif

					if (GET_HIGH_BYTE(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y) != high_byte)
						cycle_count -= 1;

					cpu_regs.reg_A = tmp_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* ldx - load X register */
		case 0xA2:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					cpu_regs.reg_X = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("a2 %.2x     |  ldx #$%.2x                  ", cpu_regs.reg_X, cpu_regs.reg_X);
					#endif

					(cpu_regs.reg_X == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_X, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xA6:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("a6 %.2x     |  ldx $%.2x = #$%.2x            ", low_byte, low_byte, tmp_byte);
					#endif

					cpu_regs.reg_X = tmp_byte;
					(cpu_regs.reg_X == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_X, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xAE:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 1);
					#ifdef DEBUG
					printf("ae %.2x %.2x  |  ldx $%.2x%.2x = #$%.2x          ", low_byte, high_byte, high_byte, low_byte, tmp_byte);
					#endif

					cpu_regs.reg_X = tmp_byte;
					(cpu_regs.reg_X == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_X, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xBE:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, 1);
					#ifdef DEBUG
					printf("be %.2x %.2x  |  ldx $%.2x%.2x,Y = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, tmp_byte);
					#endif

					if (GET_HIGH_BYTE(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y) != high_byte)
						cycle_count -= 1;

					cpu_regs.reg_X = tmp_byte;
					(cpu_regs.reg_X == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_X, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* ldy - load Y register */
		case 0xA0:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					cpu_regs.reg_Y = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("a0 %.2x     |  ldy #$%.2x                  ", cpu_regs.reg_Y, cpu_regs.reg_Y);
					#endif

					(cpu_regs.reg_Y == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_Y, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xA4:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("a4 %.2x     |  ldy $%.2x                   ", low_byte, low_byte);
					#endif

					cpu_regs.reg_Y = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					(cpu_regs.reg_Y == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_Y, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xB4:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X, 1);
					#ifdef DEBUG
					printf("b4 %.2x     |  ldy $%.2x,X = $%.4x = #$%.2x  ", low_byte, low_byte,
																			BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X, tmp_byte);
					#endif

					cpu_regs.reg_Y = tmp_byte;
					(cpu_regs.reg_Y == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_Y, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xAC:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("ac %.2x %.2x  |  ldy $%.2x%.2x                 ", low_byte, high_byte, high_byte, low_byte);
					#endif

					cpu_regs.reg_Y = read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 1);
					(cpu_regs.reg_Y == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_Y, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xBC:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, 1);
					#ifdef DEBUG
					printf("bc %.2x %.2x  |  ldy $%.2x%.2x,X = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, tmp_byte);
					#endif

					if (GET_HIGH_BYTE(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X) != high_byte)
						cycle_count -= 1;

					cpu_regs.reg_Y = tmp_byte;
					(cpu_regs.reg_Y == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_Y, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* lsr - logical shift right */
		case 0x4A:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("4a        |  lsr                       ");
					#endif

					UNSET_BIT(cpu_flags, CPU_FLAG_N);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_C)) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					cpu_regs.reg_A = (cpu_regs.reg_A >> 1) & ~0x80; // Unset last bit
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					break;
		}

		case 0x46:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 5;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("46 %.2x     |  lsr $%.2x = #$%.2x            ", low_byte, low_byte, tmp_byte);
					#endif

					UNSET_BIT(cpu_flags, CPU_FLAG_N);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_C)) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					tmp_byte = (tmp_byte >> 1) & ~0x80; // Unset last bit
					write_cpu_memory(BYTE_ADDR(low_byte, 0x00), tmp_byte);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					break;
		}

		case 0x4E:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 6;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 1);
					#ifdef DEBUG
					printf("4e %.2x %.2x  |  lsr $%.2x%.2x = #$%.2x        ", low_byte, high_byte, high_byte, low_byte, tmp_byte);
					#endif

					UNSET_BIT(cpu_flags, CPU_FLAG_N);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_C)) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					tmp_byte = (tmp_byte >> 1) & ~0x80; // Unset last bit
					write_cpu_memory(BYTE_ADDR(low_byte, high_byte), tmp_byte);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					break;
		}

		/* nop - no operation */

		/* ora - bitwise or with accumulator */
		case 0x09:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("09 %.2x     |  ora #$%.2x                  ", low_byte, low_byte);
					#endif

					cpu_regs.reg_A |= low_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x05:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("05 %.2x     |  ora $%.2x = #$%.2x            ", low_byte, low_byte, tmp_byte);
					#endif

					cpu_regs.reg_A |= tmp_byte;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* register instructions */
		case 0xAA:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("aa        |  tax                       ");
					#endif

					cpu_regs.reg_X = cpu_regs.reg_A;
					(cpu_regs.reg_X == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_X, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x8A:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("8a        |  txa                       ");
					#endif

					cpu_regs.reg_A = cpu_regs.reg_X;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xCA:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("ca        |  dex                       ");
					#endif
					cpu_regs.reg_X--;
					(cpu_regs.reg_X == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_X, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xE8:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("e8        |  inx                       ");
					#endif
					cpu_regs.reg_X++;
					(cpu_regs.reg_X == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_X, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xA8:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("a8        |  tay                       ");
					#endif

					cpu_regs.reg_Y = cpu_regs.reg_A;
					(cpu_regs.reg_Y == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_Y, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x98:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("98        |  tya                       ");
					#endif

					cpu_regs.reg_A = cpu_regs.reg_Y;
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x88:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("88        |  dey                       ");
					#endif
					cpu_regs.reg_Y--;
					(cpu_regs.reg_Y == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_Y, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0xC8:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("c8        |  iny                       ");
					#endif
					cpu_regs.reg_Y++;
					(cpu_regs.reg_Y == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_Y, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* rol - rotate left */
		case 0x2A:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("2a        |  rol                       ");
					#endif

					unsigned char tmp_int = BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N);
					cpu_regs.reg_A = (cpu_regs.reg_A << 1) & ~0x01; // Unset first bit
					cpu_regs.reg_A |= BIT_IS_SET(cpu_flags, CPU_FLAG_C);
					tmp_int ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x26:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 5;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);
					#ifdef DEBUG
					printf("26 %.2x     |  rol $%.2x = #$%.2x            ", low_byte, low_byte, tmp_byte);
					#endif

					unsigned char tmp_int = BIT_IS_SET(tmp_byte, CPU_FLAG_N);
					tmp_byte = (tmp_byte << 1) & ~0x01; // Unset first bit
					tmp_byte |= BIT_IS_SET(cpu_flags, CPU_FLAG_C);
					write_cpu_memory(BYTE_ADDR(low_byte, 0x00), tmp_byte);

					tmp_int ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					BIT_IS_SET(tmp_byte, CPU_FLAG_N) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x2E:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 6;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 0);
					#ifdef DEBUG
					printf("2e %.2x %.2x  |  rol $%.2x%.2x = #$%.2x            ", low_byte, high_byte, high_byte, low_byte, tmp_byte);
					#endif

					unsigned char tmp_int = BIT_IS_SET(tmp_byte, CPU_FLAG_N);
					tmp_byte = (tmp_byte << 1) & ~0x01; // Unset first bit
					tmp_byte |= BIT_IS_SET(cpu_flags, CPU_FLAG_C);
					write_cpu_memory(BYTE_ADDR(low_byte, high_byte), tmp_byte);

					tmp_int ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		/* ror - rotate right */
		case 0x6A:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("6a       |  ror                        ");
					#endif

					unsigned char tmp_int = BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_C);
					cpu_regs.reg_A = (cpu_regs.reg_A >> 1) & ~0x80; // Unset last bit of tmp_byte >> 1
					cpu_regs.reg_A |= (BIT_IS_SET(cpu_flags, CPU_FLAG_C)) ? 0x80 : 0x00;
					tmp_int ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(cpu_regs.reg_A == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(cpu_regs.reg_A, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					break;
		}

		case 0x66:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 5;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 1);

					#ifdef DEBUG
					printf("66 %.2x     |  ror $%.2x = #$%.2x            ", low_byte, low_byte, tmp_byte);
					#endif

					unsigned char tmp_int = BIT_IS_SET(tmp_byte, CPU_FLAG_C);
					tmp_byte = (tmp_byte >> 1) & ~0x80; // Unset last bit of tmp_byte >> 1
					tmp_byte |= (BIT_IS_SET(cpu_flags, CPU_FLAG_C)) ? 0x80 : 0x00;
					tmp_int ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);

					write_cpu_memory(BYTE_ADDR(low_byte, 0x00), tmp_byte);
					break;
		}

		case 0x7E:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 7;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char tmp_byte = read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, 1);

					#ifdef DEBUG
					printf("7e %.2x %.2x  |  ror $%.2x%.2x,X = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, tmp_byte);
					#endif

					unsigned char tmp_int = BIT_IS_SET(tmp_byte, CPU_FLAG_C);
					tmp_byte = (tmp_byte >> 1) & ~0x80; // Unset last bit of tmp_byte >> 1
					tmp_byte |= (BIT_IS_SET(cpu_flags, CPU_FLAG_C)) ? 0x80 : 0x00;
					tmp_int ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(tmp_byte == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					(BIT_IS_SET(tmp_byte, CPU_FLAG_N)) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);

					write_cpu_memory(BYTE_ADDR(low_byte, high_byte), tmp_byte);
					break;
		}

		/* rti - return from interrupt */
		case 0x40:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 6;

					#ifdef DEBUG
					printf("40        |  rti                       ");
					#endif
					cpu_flags = pull_cpu_stack();
					unsigned char low_byte = pull_cpu_stack();
					unsigned char high_byte = pull_cpu_stack();
					cpu_regs.reg_P = BYTE_ADDR(low_byte, high_byte);
					break;
		}

		/* rts - return from subroutine */
		case 0x60:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 6;

					#ifdef DEBUG
					printf("60        |  rts                       ");
					#endif
					unsigned char low_byte = pull_cpu_stack();
					unsigned char high_byte = pull_cpu_stack();
					cpu_regs.reg_P = BYTE_ADDR(low_byte, high_byte) + 1;
					break;
		}

		/* sbc - subtract with carry */
		case 0xE9:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 2;

					int tmp_int = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("e9 %.2x     |  sbc #$%.2x                  ", tmp_int, tmp_int);
					#endif

					// NES lacks decimal mode, so I won't bother with it here for now.
					if (BIT_IS_SET(cpu_flags, CPU_FLAG_C))
						tmp_int = cpu_regs.reg_A - tmp_int;
					else
						tmp_int = cpu_regs.reg_A - tmp_int - 1;

					((tmp_int > 127) || (tmp_int < -128)) ? SET_BIT(cpu_flags, CPU_FLAG_V) : UNSET_BIT(cpu_flags, CPU_FLAG_V);
					(tmp_int >= 0) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(tmp_int == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					BIT_IS_SET(tmp_int, CPU_FLAG_N) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					cpu_regs.reg_A = tmp_int & 0xFF;
					break;
		}

		case 0xF9:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					int tmp_int = read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, 1);
					#ifdef DEBUG
					printf("f9 %.2x %.2x  |  sbc $%.2x%.2x,Y = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, tmp_int);
					#endif

					if (GET_HIGH_BYTE(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y) != GET_HIGH_BYTE(cpu_regs.reg_P))
						cycle_count--;

					// NES lacks decimal mode, so I won't bother with it here for now.
					if (BIT_IS_SET(cpu_flags, CPU_FLAG_C))
						tmp_int = cpu_regs.reg_A - tmp_int;
					else
						tmp_int = cpu_regs.reg_A - tmp_int - 1;

					((tmp_int > 127) || (tmp_int < -128)) ? SET_BIT(cpu_flags, CPU_FLAG_V) : UNSET_BIT(cpu_flags, CPU_FLAG_V);
					(tmp_int >= 0) ? SET_BIT(cpu_flags, CPU_FLAG_C) : UNSET_BIT(cpu_flags, CPU_FLAG_C);
					(tmp_int == 0) ? SET_BIT(cpu_flags, CPU_FLAG_Z) : UNSET_BIT(cpu_flags, CPU_FLAG_Z);
					BIT_IS_SET(tmp_int, CPU_FLAG_N) ? SET_BIT(cpu_flags, CPU_FLAG_N) : UNSET_BIT(cpu_flags, CPU_FLAG_N);
					cpu_regs.reg_A = tmp_int & 0xFF;
					break;
		}

		/* sta - store accumulator */
		case 0x85:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("85 %.2x     |  sta $%.2x = #$%.2x            ", low_byte, low_byte,
																			read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 0));
					#endif

					write_cpu_memory(BYTE_ADDR(low_byte, 0x00), cpu_regs.reg_A);
					break;
		}

		case 0x95:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("95 %.2x     |  sta $%.2x,X = $%.4x = #$%.2x  ", low_byte, low_byte,
																			BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X,
																			read_cpu_memory(BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X, 0));
					#endif

					write_cpu_memory(BYTE_ADDR(low_byte, 0x00) + cpu_regs.reg_X, cpu_regs.reg_A);
					break;
		}

		case 0x8D:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("8d %.2x %.2x  |  sta $%.2x%.2x = #$%.2x          ", low_byte, high_byte, high_byte, low_byte,
																				read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 0));
					#endif

					write_cpu_memory(BYTE_ADDR(low_byte, high_byte), cpu_regs.reg_A);
					break;
		}

		case 0x9D:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 5;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("9d %.2x %.2x  |  sta $%.2x%.2x,X = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X,
																				read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, 0));
					#endif

					write_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_X, cpu_regs.reg_A);
					break;
		}

		case 0x99:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 5;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("99 %.2x %.2x  |  sta $%.2x%.2x,Y = $%.4x = #$%.2x", low_byte, high_byte, high_byte, low_byte,
																				BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y,
																				read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, 0));
					#endif

					write_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, cpu_regs.reg_A);
					break;
		}

		case 0x91:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 6;

					unsigned char low_byte_indir = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					unsigned char high_byte_indir = low_byte_indir + 1;
					unsigned char low_byte = read_cpu_memory(BYTE_ADDR(low_byte_indir, 0x00), 1);
					unsigned char high_byte = read_cpu_memory(BYTE_ADDR(high_byte_indir, 0x00), 1);
					#ifdef DEBUG
					printf("91 %.2x     |  sta ($%.2x),Y = $%.4x = #$%.2x", low_byte_indir, low_byte_indir, BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y,
																			read_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, 0));
					#endif

					write_cpu_memory(BYTE_ADDR(low_byte, high_byte) + cpu_regs.reg_Y, cpu_regs.reg_A);
					break;
		}

		/* stack instructions */
		case 0x9A:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 2;

					#ifdef DEBUG
					printf("9a        |  txs                       ");
					#endif

					cpu_regs.reg_S = cpu_regs.reg_X;
					break;
		}

		case 0x48:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 3;

					#ifdef DEBUG
					printf("48        |  pha                       ");
					#endif

					push_cpu_stack(cpu_regs.reg_A);
					break;
		}

		case 0x68:
		{
					cpu_regs.reg_P += 1;
					cycle_count -= 4;

					#ifdef DEBUG
					printf("68        |  pla                       ");
					#endif
					unsigned char low_byte = pull_cpu_stack();
					cpu_regs.reg_A = low_byte;
					break;
		}

		/* stx - store X register */
		case 0x86:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("86 %.2x     |  stx $%.2x = #$%.2x            ", low_byte, low_byte,
																			read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 0));
					#endif

					write_cpu_memory(BYTE_ADDR(low_byte, 0x00), cpu_regs.reg_X);
					break;
		}

		case 0x8E:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("83 %.2x %.2x  |  stx $%.2x%.2x = #$%.2x          ", low_byte, high_byte, high_byte, low_byte,
																				read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 0));
					#endif

					write_cpu_memory(BYTE_ADDR(low_byte, high_byte), cpu_regs.reg_X);
					break;
		}

		/* sty - store Y register */
		case 0x84:
		{
					cpu_regs.reg_P += 2;
					cycle_count -= 3;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("84 %.2x     |  sty $%.2x = #$%.2x            ", low_byte, low_byte,
																			read_cpu_memory(BYTE_ADDR(low_byte, 0x00), 0));
					#endif

					write_cpu_memory(BYTE_ADDR(low_byte, 0x00), cpu_regs.reg_Y);
					break;
		}

		case 0x8C:
		{
					cpu_regs.reg_P += 3;
					cycle_count -= 4;

					unsigned char low_byte = read_cpu_memory(cpu_regs.reg_P - 2, 1);
					unsigned char high_byte = read_cpu_memory(cpu_regs.reg_P - 1, 1);
					#ifdef DEBUG
					printf("8c %.2x %.2x  |  sty $%.2x%.2x = #$%.2x          ", low_byte, high_byte, high_byte, low_byte,
																				read_cpu_memory(BYTE_ADDR(low_byte, high_byte), 0));
					#endif

					write_cpu_memory(BYTE_ADDR(low_byte, high_byte), cpu_regs.reg_Y);
					break;
		}

		default:
					cycle_count -= 1;
					fprintf(stderr, "Unknown opcode %.2x.\n", cpu_memory[cpu_regs.reg_P]);
	}

	#ifdef DEBUG
	printf("  |  A: %.2x  X: %.2x  Y: %.2x  F: ", cpu_regs.reg_A, cpu_regs.reg_X, cpu_regs.reg_Y);
	printf("%s%s-%s%s%s%s%s\n",
			BIT_IS_SET(cpu_flags, CPU_FLAG_N) ? "N" : "n",
			BIT_IS_SET(cpu_flags, CPU_FLAG_V) ? "V" : "v",
			BIT_IS_SET(cpu_flags, CPU_FLAG_B) ? "B" : "b",
			BIT_IS_SET(cpu_flags, CPU_FLAG_D) ? "D" : "d",
			BIT_IS_SET(cpu_flags, CPU_FLAG_I) ? "I" : "i",
			BIT_IS_SET(cpu_flags, CPU_FLAG_Z) ? "Z" : "z",
			BIT_IS_SET(cpu_flags, CPU_FLAG_C) ? "C" : "c");
	#endif

	return cycle_count;
}
