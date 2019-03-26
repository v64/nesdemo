#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DEBUG

// Some taken from Mario disassembly

// CPU Flags
#define CPU_FLAG_C               0x01
#define CPU_FLAG_Z               0x02
#define CPU_FLAG_I               0x04
#define CPU_FLAG_D               0x08
#define CPU_FLAG_B               0x10
// 0x20 is unused
#define CPU_FLAG_V               0x40
#define CPU_FLAG_N               0x80

// Top down stack, 0x0100 to 0x01FF
#define STACK_START              0xFF
#define STACK_END                0x00

#define RAM_MIRROR_START         0x0800
#define RAM_MIRROR_END           0x1FFF
#define REG_MIRROR_START         0x2008
#define REG_MIRROR_END           0x3FFF
#define PPU_TABLE_MIRROR_START   0x3000
#define PPU_TABLE_MIRROR_END     0x3EFF
#define PPU_PALETTE_MIRROR_START 0x3F20
#define PPU_PALETTE_MIRROR_END   0x3FFF

#define LOWER_PRG_ROM            0x8000
#define UPPER_PRG_ROM            0xC000
#define PRG_ROM_SIZE             0x4000
#define CHR_ROM1                 0x0000
#define CHR_ROM2                 0x1000
#define CHR_ROM_SIZE             0x2000

// PPU Memory stuff
#define PPU_NAME_TABLE0          0x2000
#define PPU_NAME_TABLE1          0x2400
#define PPU_NAME_TABLE2          0x2800
#define PPU_NAME_TABLE3          0x2C00
#define TOTAL_TILES              960

// Controller bits
#define CTRLER1                  0x4016
#define CTRLER2                  0x4017
#define BUTTON_A                 0x01
#define BUTTON_B                 0x02
#define BUTTON_SELECT            0x04
#define BUTTON_START             0x08
#define BUTTON_UP                0x10
#define BUTTON_DOWN              0x20
#define BUTTON_LEFT              0x40
#define BUTTON_RIGHT             0x80
#define BUTTON_PRESSED           0x41
#define BUTTON_UNPRESSED         0x40

// PPU registers
#define PPU_CTRL_REG1            0x2000
#define PPU_NAME_TABLE_ADDR1     0x01
#define PPU_NAME_TABLE_ADDR2     0x02
#define PPU_VERTICAL_WRITE       0x04
#define PPU_SPRITE_ADDR          0x08
#define PPU_SCREEN_ADDR          0x10
#define PPU_SPRITE_SIZE          0x20
// #define PPU_MASTER_SLAVE      0x40, Unused in NES
#define PPU_VBLANK_INT           0x80

#define PPU_CTRL_REG2            0x2001
#define PPU_MONOCHROME           0x01
#define PPU_IMG_MASK             0x02
#define PPU_SPRITE_MASK          0x04
#define PPU_BACKGROUND_ENABLE    0x08
#define PPU_SPRITE_ENABLE        0x10
// 0 = black, 1 = blue, 2 = green, 4 = read
#define PPU_BGCOLOR1             0x20
#define PPU_BGCOLOR2             0x40
#define PPU_BGCOLOR3             0x80

#define PPU_STATUS               0x2002
// bits 0-3 are unknown
#define PPU_IGNORE_WRITES        0x10
#define PPU_SPRITE_COUNT         0x20
#define PPU_HIT_FLAG             0x40
#define PPU_IN_VBLANK            0x80

#define PPU_SPRITE_MEM_ADDR      0x2003

#define PPU_SPRITE_MEM_DATA      0x2004

#define PPU_SCROLL_OFFSETS       0x2005

#define PPU_MEM_ADDR             0x2006

#define PPU_MEM_DATA             0x2007

// Interrupt locations
#define NMI_INT_LOW              0xFFFA
#define NMI_INT_HIGH             0xFFFB
#define RESET_INT_LOW            0xFFFC
#define RESET_INT_HIGH           0xFFFD
#define BRK_INT_LOW              0xFFFE
#define BRK_INT_HIGH             0xFFFF

#define CPU_MEMORY_SIZE          0xFFFF+1
#define PPU_MEMORY_SIZE          0xFFFF+1

#endif
