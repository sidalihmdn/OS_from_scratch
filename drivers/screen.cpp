#include "screen.h"
#include "../io_functions/low_level.h"

// declarations
void parse_offset(int &col , int &row , int offset);
int get_offset(int col, int row);
int get_cursor();
void set_cursor(int offset);
void set_cursor(int offset);
//#################################################

/// @brief this function prints a carater into the screen
/// @param attribute forground and background colors
/// @param col the column number
/// @param row the row number
/// @param character the caracter that we wish to print
void print_char(char attribute, int col, int row , char character){
    unsigned char *video_address = (unsigned char*) VIDEO_ADDRESS;

    // if there's no attribute we'll use the default white on black
    if(!attribute){
        attribute = WHITE_ON_BLACK;
    }

    int offset;
    if (col >= 0 && row >= 0){
        offset = get_offset(col,row);
    }
    else{
        offset =get_cursor();
    }

    if(character=='\n'){
        set_cursor(offset+(MAX_COL*2-offset%(MAX_COL*2)));
    }
    else{
        video_address[offset] = character;
        video_address[offset+1] = attribute;
        // change the cursor pos
        set_cursor(offset+2);
    }
}

void zprint(char *string){
int i = 0;
  while(string[i]!='\0'){
    print_char(WHITE_ON_BLACK,-1,-1, string[i]);
    i++;
  }
}

void print_string(char string[], int len){
    int offset, col, row;
    for(int i=0 ; i < len ; i++){
        //offset = get_cursor();
        //col = offset%(MAX_COL)/2;
        //row = offset/(2*MAX_COL);
        print_char(WHITE_ON_BLACK,-1 , -1 , string[i]);
    }
}

/// @brief calculate the offset given a column and row number
/// @param col is the column number
/// @param row is the row number
/// @return the offset
int get_offset(int col, int row){
    return (row*MAX_COL+col)*2;
}

/// @brief this function will return the cursor position
/// @return the offset is the number of memory case between the cursor and the address 0xb8000
int get_cursor(){
    int offset;
    outb(REG_SCREEN_CTRL, 0x0E); // the index 0x0e (14) is the the cursor high position
    offset = inb(REG_SCREEN_DATA) << 8 ; // in order to get the high byte value
    outb(REG_SCREEN_CTRL, 0x0F); // the index 0x0f (15) is the the cursor low positon
    offset += inb(REG_SCREEN_DATA);
    return offset*2;
}

/// @brief this function will set the cursor at a given x , y postions
/// @param x is the column number (shouldn't be greater than MAX_COL)
/// @param y is the row number (shouldn't be greater than MAX_ROX)
void set_cursor(int offset){
    offset/=2;
    outb(REG_SCREEN_CTRL, 0x0E);
    outb(REG_SCREEN_DATA, (unsigned char)(offset >> 8)); // setting the high byte
    outb(REG_SCREEN_CTRL, 0x0F);
    outb(REG_SCREEN_DATA, (unsigned char)(offset & 0xFF)); // setting the low byte
}



/// @brief this functions will replace every caracter on the screen with a blanc
void clean_screen(){
    int screen_size = MAX_ROW * MAX_COL;
    char *screen = (char*) VIDEO_ADDRESS;
    for(int i = 0 ; i < screen_size; i++){
        screen[2*i] = ' ';
        screen[2*i+1] =WHITE_ON_BLACK;
    }
    set_cursor(0);
}


void parse_offset(int& col , int& row , int offset){
    row = offset/(2*MAX_COL);
    col = offset%(MAX_COL)/2;
}
