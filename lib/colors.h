#ifndef __COLORS__

#define __COLORS__

#include "./config.h"

void add_to_display_buffer(const char *string, char *output, ...);
void no_output_string_display(const char *string, ...);
void display(const char *string);

void Default_Colors(config_type config, char *string);
void Reset_Colors(config_type config, char *string);
void Border_Colors(config_type config, char *string);
void Header_Text_Colors(config_type config, char *string);
void Odd_Text_Colors(config_type config, char *string);
void Even_Text_Colors(config_type config, char *string);
void Selection_Content_Colors(config_type config, char *string);

void Hide_Cursor(char *string);
void Show_Cursor(char *string);

void Clear_Screen_By_Scrolldown();
void Clear_Screen_By_Cleaning();
void Scrollback_To_Screen_Start();

#endif