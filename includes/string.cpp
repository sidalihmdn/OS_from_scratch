#include "string.h"

/// @brief this function will return the lenght of a given string
/// @param c the address of the first character of the string
/// @return the length of the string
int len(char* c){
    int i = 0;
    while (c[i]='\0'){
        i++;
    }
    return i;
}

/// @brief calculate the number of digits in a given number
/// @param number 
/// @return the number of digits (example 1234 -> 4)
int nb_digits(int number){
    int digits=0;
    while(number > 0){
        ++digits;
        number=number/10;
    }
    return digits;
}

// this function will return the number of hex digits (if we can call them digits)
int nb_hex(int number){
  int nbHex=0;
  while (number>0){
    ++nbHex;
    number=number/16;
  }
  return nbHex;
}

/// @brief return the asci code of a given number (0-9)
/// @param a 
/// @return 
char int2char(int a){
    return '0' + a ;
}

/// @brief transforms a given number into string
/// @param a 
/// @return 
char *int2String(int a){
    int digits_number = nb_digits(a);
    char *c;
    int i = 1;
    while(a > 0){
        c[digits_number-i]= int2char(a%10);
        a = a/10 ;
        i++;
    }
    c[digits_number] = '\0';
    return c;
}


char hex2char(int hex){
    return '0' + hex;
}

char *hex2String(int hex){
  int nbHex = nb_hex(hex);
  char *c;
  c[0] = '0';
  c[1] = 'x';
  int i = 1;
  while(hex > 0){
    c[2+nbHex-i] = hex2char(hex%16);
    hex = hex/16;
    i++;
  }
  c[nbHex+2]='\0';
  return c;
}
