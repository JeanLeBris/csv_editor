#ifndef __UTILS__

#define __UTILS__

#include "constants.h"
#include "config.h"

void Set_Locale(config_type config);
int utf8_strlen(char* string);
int encoding_strlen(char* string, config_type config);
int length_of_first_character(char* string, config_type config);
int length_of_last_character(char* string, config_type config);
int displayed_length_to_logical_length_forward(char* string, int displayed_length, config_type config);
int displayed_length_to_logical_length_backward(char* string, int displayed_length, config_type config);

int Is_Integer(char *value, int size);
int Is_Decimal(char *value, int size);
int Is_Date(char *value, int size);

#endif