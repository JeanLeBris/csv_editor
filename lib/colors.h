#ifndef __COLORS__

#define __COLORS__

#include "./config.h"

void display(const char *string, char *output, ...);
void no_output_string_display(const char *string, ...);

void Default_Colors(config_type config, char *string);
void Reset_Colors(config_type config, char *string);
void Odd_Line_Colors(config_type config, char *string);
void Even_Line_Colors(config_type config, char *string);
void Selection_Content_Colors(config_type config, char *string);

void S_Default_Colors(config_type config, char *string);
void S_Reset_Colors(config_type config, char *string);
void S_Odd_Line_Colors(config_type config, char *string);
void S_Even_Line_Colors(config_type config, char *string);
void S_Selection_Content_Colors(config_type config, char *string);

#ifdef __linux__
void Hide_Cursor(char *string);
void Show_Cursor(char *string);
#endif
#ifdef _WIN64
void S_Hide_Cursor(char *string);
void S_Show_Cursor(char *string);
#endif

void Clear_Screen_By_Scrolldown();
void Clear_Screen_By_Cleaning();
void Scrollback_To_Screen_Start();

#endif