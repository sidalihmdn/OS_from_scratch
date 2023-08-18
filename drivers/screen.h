#define VIDEO_ADDRESS 0xb8000
#define MAX_ROW 25
#define MAX_COL 80

#define WHITE_ON_BLACK 0x0f

#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

void set_cursor(int x, int y);
int get_offset(int col, int row);
void clean_screen();
void print_char(char attribute, int col, int row , char character);