#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef  _WIN64
#include <windows.h>
#endif
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

void S_Hide_Cursor(char *string){
    strcat(string, "\e[?25l");
}

void S_Show_Cursor(char *string){
    strcat(string, "\e[?25h");
}

void Clear_Screen_By_Scrolldown(){
    printf("\e[1;1H\e[2J");
    // system("cls");
}

void Clear_Screen_By_Cleaning(){
    HANDLE hStdout;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    SMALL_RECT scrollRect;
    COORD scrollTarget;
    CHAR_INFO fill;

    // Get the number of character cells in the current buffer.
    if (!GetConsoleScreenBufferInfo(hStdout, &csbi))
    {
        return;
    }

    // Scroll the rectangle of the entire buffer.
    scrollRect.Left = 0;
    scrollRect.Top = 0;
    scrollRect.Right = csbi.dwSize.X;
    scrollRect.Bottom = csbi.dwSize.Y;

    // Scroll it upwards off the top of the buffer with a magnitude of the entire height.
    scrollTarget.X = 0;
    scrollTarget.Y = (SHORT)(0 - csbi.dwSize.Y);

    // Fill with empty spaces with the buffer's default text attribute.
    fill.Char.UnicodeChar = TEXT(' ');
    fill.Attributes = csbi.wAttributes;

    // Do the scroll
    ScrollConsoleScreenBuffer(hStdout, &scrollRect, NULL, scrollTarget, &fill);

    // Move the cursor to the top left corner too.
    csbi.dwCursorPosition.X = 0;
    csbi.dwCursorPosition.Y = 0;

    SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
}

void Scrollback_To_Screen_Start(){
    HANDLE hStdout;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    // Get the number of character cells in the current buffer.
    if (!GetConsoleScreenBufferInfo(hStdout, &csbi))
    {
        return;
    }

    // Move the cursor to the top left corner too.
    csbi.dwCursorPosition.X = 0;
    csbi.dwCursorPosition.Y = 0;

    SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
}