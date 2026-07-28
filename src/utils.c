#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#ifdef _WIN64
#include <windows.h>
#endif

#include "../lib/utils.h"

void Set_Locale(config_type config){
    if(config->encoding == ENCODING_ASCII){
        #ifdef _WIN64
        SetConsoleOutputCP(CP_UTF8);
        #endif
        setlocale(LC_ALL, "ANSI");
    }
    else if(config->encoding == ENCODING_UTF8){
        #ifdef _WIN64
        SetConsoleCP(CP_UTF8);      // Last added, need to check the effect
        SetConsoleOutputCP(CP_UTF8);
        setlocale(LC_ALL, "UTF-8");
        #endif
        #ifdef __linux__
        setlocale(LC_ALL, "");
        #endif
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

int length_of_first_character(char* string, config_type config){
    int output = 0;
    switch(config->encoding){
        case ENCODING_ASCII:
            output = 1;
            break;
        case ENCODING_UTF8:
            output = 1;
            if((*string & 0xC0) == 0xC0){
                for(char* character = &string[1]; !(char)((*character + 0x80) & 0xC0); character = &character[1]){
                    output++;
                }
            }
            break;
        default:
            output = 1;
            break;
    }
    return output;
}

int length_of_last_character(char* string, config_type config){
    int output = 0;
    switch(config->encoding){
        case ENCODING_ASCII:
            output = 1;
            break;
        case ENCODING_UTF8:
            output = 1;
            for(char* character = &string[-1]; !(char)((*character + 0x80) & 0xC0); character = &character[-1]){
                output++;
            }
            break;
        default:
            output = 1;
            break;
    }
    return output;
}

int displayed_length_to_logical_length_forward(char* string, int displayed_length, config_type config){
    int output = 0;
    for(int i = 0; i < displayed_length; i++){
        output += length_of_first_character(&string[output], config);
    }
    return output;
}

int displayed_length_to_logical_length_backward(char* string, int displayed_length, config_type config){
    int output = 0;
    for(int i = 0; i < displayed_length - 1; i++){
        output += length_of_last_character(&string[-output], config);
    }
    output += length_of_first_character(&string[0], config);
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
 * Accepted formats :
 * - YYYY-MM-DD hh:mm:ss
 * - DD/MM/YYYY
 */
int Is_Date(char *value, int size){
    // for(int i = 0; i < size; i++){
    //     if(i == 4 || i == 7){
    //         if(value[i] != '-')
    //             return 0;
    //     }
    //     else if(i == 10){
    //         if(value[i] != ' ')
    //             return 0;
    //     }
    //     else if(i == 13 || i == 16){
    //         if(value[i] != ':')
    //             return 0;
    //     }
    //     else if(i == 19){
    //         if(value[i] != '\0')
    //             return 0;
    //     }
    //     else{
    //         if(value[i] < 0x30 || value[i] > 0x39)
    //             return 0;
    //     }
    // }
    for(int i = 0; i < size; i++){
        if(i == 2 || i == 5){
            if(value[i] != '/')
                return 0;
        }
        else if(i == 10){
            if(value[i] != '\0')
                return 0;
        }
        else{
            if((value[i] < 0x30 || value[i] > 0x39) && value[i] != '?')
                return 0;
        }
    }
    return 1;
}