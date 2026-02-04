#ifndef __UTILS__

#define __UTILS__

#include "constants.h"
#include "config.h"

void Set_Locale(config_type config);
int utf8_strlen(char* string);
int encoding_strlen(char* string, config_type config);

int Is_Integer(char *value, int size);
int Is_Decimal(char *value, int size);
int Is_Date(char *value, int size);

#endif