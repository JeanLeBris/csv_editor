#ifndef __CONFIG__

#define __CONFIG__

typedef struct config_type{
    char installation_directory[101];

    int default_background_color;
    int default_text_color;
    int background_header_color;
    int text_header_color;
    int odd_background_line_color;
    int odd_text_line_color;
    int even_background_line_color;
    int even_text_line_color;
    int selection_background_color;
    int selection_text_color;

    int table_type;
    int file_line_max_length;
    int file_max_length;
    int unfocused_cell_max_width;
    int focused_cell_max_width;
    int cell_max_width;
    int max_table_length;

    char input_mode[10];
    char input_file[50];

    int window_width;
    int window_length;
}*config_type;

void Set_Default_Config(config_type config, char *exe_path);
void Load_Config(config_type config);
void Get_Config_From_Args(config_type config, int argc, char **argv);
void Get_Window_Size(config_type config);
void Set_Max_Table_Length(config_type config);
void Print_Config(config_type config);

#endif