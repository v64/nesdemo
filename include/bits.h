#ifndef BITS_H
#define BITS_H

// Simple stuff for bit/byte manipulation

#define BIT_IS_SET(DATA, BIT) (((DATA) & (BIT)) ? 1 : 0)
#define SET_BIT(DATA, BIT) ((DATA) |= (BIT))
#define UNSET_BIT(DATA, BIT) ((DATA) &= ~(BIT))

#define BYTE_ADDR(LOW, HIGH) ((HIGH << 8) + LOW)
#define GET_HIGH_BYTE(ADDR) (((ADDR) & 0xFF00) >> 8)
#define GET_LOW_BYTE(ADDR) ((ADDR) & 0xFF)

#endif
