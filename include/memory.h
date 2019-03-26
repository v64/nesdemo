void load_roms_into_memory(FILE *, unsigned char, unsigned char);
unsigned char read_cpu_memory(unsigned short, int);
void write_cpu_memory(unsigned short, unsigned char);
void push_cpu_stack(unsigned char);
unsigned char pull_cpu_stack();
