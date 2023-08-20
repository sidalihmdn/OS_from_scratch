/// @brief this function will return the lenght of a given string
/// @param c the address of the first character of the string
/// @return the length of the string
int len(char* c){
    int i = 0;
    while (c[i]=!0){
        ++i;
    }
    return i;
}

int nb_digits(int number){
    int digits;
    while(number > 0){
        ++digits;
        number=number/10;
    }
    return digits;
}
char int2char(int a){
    return '0' + a ;
}

char *int2String(int a){
    int digits_number = nb_digits(a);
    char *c;
    int i = 0;
    while(a > 0){
        i++;
        c[digits_number-i+1]= int2char(a%10);
        a = a/10 ;
    }
    c[digits_number+1] = 0;
    return c;
}


char *hex2char(char hex){
    return 0;
}

