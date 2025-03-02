#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <ncurses.h>
#endif
#ifdef  _WIN64
#include <windows.h>
#endif
#include "../lib/colors.h"

#ifdef __linux__
void add_to_display_buffer(const char *string, char *output, ...){
    printw(string);
}

void no_output_string_display(const char *string, ...){
    printw(string);
    refresh();
}

void display(const char *string){
    refresh();
}

void Default_Colors(config_type config, char *string){
    attron(COLOR_PAIR(1));
}

void Reset_Colors(config_type config, char *string){
    Default_Colors(config, string);
}

void Odd_Line_Colors(config_type config, char *string){
    attron(COLOR_PAIR(3));
}

void Even_Line_Colors(config_type config, char *string){
    attron(COLOR_PAIR(4));
}

void Selection_Content_Colors(config_type config, char *string){
    attron(COLOR_PAIR(5));
}

void Hide_Cursor(char *string){
    curs_set(0);
}

void Show_Cursor(char *string){
    curs_set(1);
}

void Clear_Screen_By_Scrolldown(){
    system("clear");
}

void Clear_Screen_By_Cleaning(){
    system("clear");
}

void Scrollback_To_Screen_Start(){
    clear();
}
#endif

#ifdef _WIN64
void add_to_display_buffer(const char *string, char *output, ...){
    strcat(output, string);
}

void no_output_string_display(const char *string, ...){
    printf(string);
}

void display(const char *string){
    printf("%s", string);
}

void Default_Colors(config_type config, char *string){
    char buffer[20] = "";
    sprintf(buffer, "\e[%d;%dm", config->default_background_color, config->default_text_color);
    strcat(string, buffer);
}

void Reset_Colors(config_type config, char *string){
    Default_Colors(config, string);
}

void Odd_Line_Colors(config_type config, char *string){
    char buffer[20] = "";
    sprintf(buffer, "\e[%d;%dm", config->odd_background_line_color, config->odd_text_line_color);
    strcat(string, buffer);
}

void Even_Line_Colors(config_type config, char *string){
    char buffer[20] = "";
    sprintf(buffer, "\e[%d;%dm", config->even_background_line_color, config->even_text_line_color);
    strcat(string, buffer);
}

void Selection_Content_Colors(config_type config, char *string){
    char buffer[20] = "";
    sprintf(buffer, "\e[%d;%dm", config->selection_background_color, config->selection_text_color);
    strcat(string, buffer);
}

void Hide_Cursor(char *string){
    strcat(string, "\e[?25l");
}

void Show_Cursor(char *string){
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
#endif