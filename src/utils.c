#include <stdio.h>
#include <stdlib.h>

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

int Is_Date(char *value, int size){

}