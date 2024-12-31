#ifndef __COLORS__

#define __COLORS__

#include "./config.h"

void Default_Colors(config_type config);
void Reset_Colors(config_type config);
void Odd_Line_Colors(config_type config);
void Even_Line_Colors(config_type config);
void Selection_Content_Colors(config_type config);

void S_Default_Colors(config_type config, char *string);
void S_Reset_Colors(config_type config, char *string);
void S_Odd_Line_Colors(config_type config, char *string);
void S_Even_Line_Colors(config_type config, char *string);
void S_Selection_Content_Colors(config_type config, char *string);

void Clear_Screen();

#endif