#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#ifdef _WIN64
#include <windows.h>
#endif

#include "../lib/utils.h"

void Set_Locale(config_type config){
    if(config->encoding == ENCODING_UTF8){
        #ifdef _WIN64
        SetConsoleOutputCP(CP_UTF8);
        #endif
        setlocale(LC_ALL, "UTF-8");
    }
}

int utf8_strlen(char* string){
    int count = 0;
    for(int i = 0; i < strlen(string); i++){
        if(((char)(string[i] + 0x80)) & 0xC0){
            count++;
        }
    }
    return count;
}

int encoding_strlen(char* string, config_type config){
    int output = 0;
    switch(config->encoding){
        case ENCODING_ASCII:
            output = strlen(string);
            break;
        case ENCODING_UTF8:
            output = utf8_strlen(string);
            break;
        default:
            output = strlen(string);
            break;
    }
    return output;
}

int Is_Integer(char *value, int size){
    for(int i = 0; i < size && value[i] != 0; i++){
        // If value is written with the scientific notation (XXXe+YYY or XXXe-YYY)
        // If first value is a 'e' followed by a '+' or '-' and is then followed by a number, then increase i by one so the next loop doesn't read the '+' or '-' and just umps to the number
        if(value[i] == 'e' && (value[i+1] == '+' || value[i+1] == '-') && !(value[i+2] < 0x30 || value[i+2] > 0x39)){
            i++;
        }
        // If first value is not a number and not a '-' then
        else if(i == 0 && value[i] != '-' && (value[i] < 0x30 || value[i] > 0x39)){
            return 0;
        }
        // If any other value is not a number then
        else if(i != 0 && (value[i] < 0x30 || value[i] > 0x39)){
            return 0;
        }
    }
    return 1;
}

int Is_Decimal(char *value, int size){
    int decimal_sign_found = 0;
    for(int i = 0; i < size && value[i] != 0; i++){
        // If first value is not a number, not a '-' and not a '.' then
        if(i == 0 && value[i] != '.' && value[i] != '-' && (value[i] < 0x30 || value[i] > 0x39)){
            return 0;
        }
        // If any other value is not a number and not a '.' then
        else if(i != 0 && value[i] != '.' && (value[i] < 0x30 || value[i] > 0x39)){
            return 0;
        }
        // If you found a '.' then
        else if(value[i] == '.' && decimal_sign_found == 0){
            decimal_sign_found++;
        }
        // If you found a second '.' then
        else if(value[i] == '.' && decimal_sign_found != 0){
            return 0;
        }
    }
    return 1;
}

/**
 * Check if the inputed value is a date
 * 
 * The only accepted format right now is `YYYY-MM-DD hh:mm:ss`
 */
int Is_Date(char *value, int size){
    for(int i = 0; i < size; i++){
        if(i == 4 || i == 7){
            if(value[i] != '-')
                return 0;
        }
        else if(i == 10){
            if(value[i] != ' ')
                return 0;
        }
        else if(i == 13 || i == 16){
            if(value[i] != ':')
                return 0;
        }
        else if(i == 19){
            if(value[i] != '\0')
                return 0;
        }
        else{
            if(value[i] < 0x30 || value[i] > 0x39)
                return 0;
        }
    }
    return 1;
}