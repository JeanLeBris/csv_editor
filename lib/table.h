#ifndef __TABLE__

#define __TABLE__

#include "./config.h"
#include "./colors.h"

typedef struct table_type{
    char **header;
    char ***table;
    int active_line;    // id of displayed row (as in memory but not as in the file)
    int active_column;  // id of displayed column (not as it is in memory)
    int character_highlighted;
    int first_column_printed;   // id of the first displayed column (not as it is in memory)
    int last_column_printed;
    int first_line_printed;
    int last_line_printed;
    int first_line_in_memory;
    int last_line_in_memory;
    int first_character_printed;
    int on_file_begin;
    int on_file_end;
    int *cell_width;
    int *columns_order_of_display;

    char **command;
    int command_character_highlighted;
    int active_command;

    int table_length;
    int table_width;
}*table_type;

void Get_File_Characteristics(table_type table_object, config_type config);
void Fetch_Data_From_Csv(table_type table_object, config_type config, int start_in_file, int end_in_file, int start_to_replace, int memory_to_replace);
void Print_Table(table_type table_object, config_type config);
void Update_Cell_Width_By_Column(table_type table_object, int displayed_column_id);

table_type Free_Table_Object(table_type table_object, config_type config);

#endif