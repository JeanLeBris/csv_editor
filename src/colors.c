#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/colors.h"

void Default_Colors(config_type config){
    printf("\e[%d;%dm", config->default_background_color, config->default_text_color);
}

void Reset_Colors(config_type config){
    Default_Colors(config);
}

void Odd_Line_Colors(config_type config){
    printf("\e[%d;%dm", config->odd_background_line_color, config->odd_text_line_color);
}

void Even_Line_Colors(config_type config){
    printf("\e[%d;%dm", config->even_background_line_color, config->even_text_line_color);
}

void Selection_Content_Colors(config_type config){
    printf("\e[%d;%dm", config->selection_background_color, config->selection_text_color);
}

void S_Default_Colors(config_type config, char *string){
    char buffer[20] = "";
    sprintf(buffer, "\e[%d;%dm", config->default_background_color, config->default_text_color);
    strcat(string, buffer);
}

void S_Reset_Colors(config_type config, char *string){
    S_Default_Colors(config, string);
}

void S_Odd_Line_Colors(config_type config, char *string){
    char buffer[20] = "";
    sprintf(buffer, "\e[%d;%dm", config->odd_background_line_color, config->odd_text_line_color);
    strcat(string, buffer);
}

void S_Even_Line_Colors(config_type config, char *string){
    char buffer[20] = "";
    sprintf(buffer, "\e[%d;%dm", config->even_background_line_color, config->even_text_line_color);
    strcat(string, buffer);
}

void S_Selection_Content_Colors(config_type config, char *string){
    char buffer[20] = "";
    sprintf(buffer, "\e[%d;%dm", config->selection_background_color, config->selection_text_color);
    strcat(string, buffer);
}

void Clear_Screen(){
    // printf("\e[1;1H\e[2J");
    system("cls");
}