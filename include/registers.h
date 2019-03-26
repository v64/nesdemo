#ifndef CPU_H
#define CPU_H

struct cpu_regs_list {
	unsigned char reg_A;  // Accumulator
	unsigned char reg_X;  // Gen reg 1
	unsigned char reg_Y;  // Gen reg 2
	unsigned char reg_S;  // Stack pointer
	unsigned short reg_P; // Program counter
};

#endif
