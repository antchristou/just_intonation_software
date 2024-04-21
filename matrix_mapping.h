
#define ROWS 16
#define COLUMNS 32


extern int LOOKUP_TABLE[ROWS][COLUMNS];

void generate_mapping();

void print_mapping();

int lookup(int x,int y);
