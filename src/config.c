#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <sys/ioctl.h>
#include <unistd.h>
#include <ncurses.h>
#endif
#ifdef _WIN64
#include <libloaderapi.h>
#include <windows.h>
#endif
#include "../lib/config.h"
#include "../lib/colors.h"

void Set_Default_Config(config_type config, char *exe_path){
    // char memstr[101] = "";
    // int memstr_size = GetModuleFileNameA(NULL, memstr, 100);
    // strncpy(config->installation_directory, memstr, memstr_size - 14);
    #ifdef __linux__
    getcwd(config->installation_directory, 100);    // only works if in the program's directory
    config->installation_directory[strlen(config->installation_directory) + 1] = '\0';
    config->installation_directory[strlen(config->installation_directory)] = '/';
    #endif
    #ifdef _WIN64
    strncpy(config->installation_directory, exe_path, strlen(exe_path) - 14);
    config->installation_directory[strlen(exe_path) - 14] = '\0';
    #endif

    config->default_background_color = 40;
    config->default_text_color = 37;
    config->text_border_color = 37;
    config->background_header_color = 40;
    config->text_header_color = 37;
    config->odd_background_line_color = 40;
    config->odd_text_line_color = 37;
    config->even_background_line_color = 40;
    config->even_text_line_color = 37;
    config->selection_background_color = 47;
    config->selection_text_color = 30;

    #ifdef __linux__
    init_pair(1, config->default_text_color / 10 < 4 ? config->default_text_color%10 : 8 + config->default_text_color%10, config->default_background_color / 10 < 5 ? config->default_background_color%10 : 8 + config->default_background_color%10);
    init_pair(2, config->default_text_color / 10 < 4 ? config->default_text_color%10 : 8 + config->default_text_color%10, 0);
    init_pair(3, config->text_header_color / 10 < 4 ? config->text_header_color%10 : 8 + config->text_header_color%10, config->background_header_color / 10 < 5 ? config->background_header_color%10 : 8 + config->background_header_color%10);
    init_pair(4, config->odd_text_line_color / 10 < 4 ? config->odd_text_line_color%10 : 8 + config->odd_text_line_color%10, config->odd_background_line_color / 10 < 5 ? config->odd_background_line_color%10 : 8 + config->odd_background_line_color%10);
    init_pair(5, config->even_text_line_color / 10 < 4 ? config->even_text_line_color%10 : 8 + config->even_text_line_color%10, config->even_background_line_color / 10 < 5 ? config->even_background_line_color%10 : 8 + config->even_background_line_color%10);
    init_pair(6, config->selection_text_color / 10 < 4 ? config->selection_text_color%10 : 8 + config->selection_text_color%10, config->selection_background_color / 10 < 5 ? config->selection_background_color%10 : 8 + config->selection_background_color%10);
    #endif
    
    config->table_type = 0; // static
    config->file_line_max_length = 1000;
    config->file_max_length = 1000;
    config->unfocused_cell_max_width = 30;
    config->focused_cell_max_width = 30;
    config->cell_max_width = 100;
    config->input_separator = ',';
    config->output_separator = ',';

    config->commands_history_length = 100;

    strcpy(config->input_file, "");

    config->show_config = 0;

    Set_Window_Size(config);
}

void Load_Config(config_type config){
    char config_file_path[101] = "";
    char line[101] = "";
    char *fragmented_line = NULL;
    char seps[] = " \n=\"";
    strcat(strcat(config_file_path, config->installation_directory), "./csv_editor.conf");
    FILE *f = fopen(config_file_path, "r");
    if(f == NULL){
        printf("file error\n");
        return;
    }
    fseek(f, 0, SEEK_SET);
    while(!feof(f)){
        fgets(line, 100, f);
        // printf(line);
        fragmented_line = strtok(line, seps);
        // printf("%s : ", fragmented_line);
        if(strcmp(fragmented_line, "default_background_color") == 0){
            // strcpy(config->default_background_color, strtok(NULL, seps));
            config->default_background_color = atoi(strtok(NULL, seps));
            // printf("%s\n", config->default_background_color);
        }
        else if(strcmp(fragmented_line, "default_text_color") == 0){
            // strcpy(config->default_text_color, strtok(NULL, seps));
            config->default_text_color = atoi(strtok(NULL, seps));
            // printf("%s\n", config->default_text_color);
        }
        else if(strcmp(fragmented_line, "text_border_color") == 0){
            // strcpy(config->default_text_color, strtok(NULL, seps));
            config->text_border_color = atoi(strtok(NULL, seps));
            // printf("%s\n", config->default_text_color);
        }
        else if(strcmp(fragmented_line, "background_header_color") == 0){
            // strcpy(config->background_header_color, strtok(NULL, seps));
            config->background_header_color = atoi(strtok(NULL, seps));
            // printf("%s\n", config->background_header_color);
        }
        else if(strcmp(fragmented_line, "text_header_color") == 0){
            // strcpy(config->text_header_color, strtok(NULL, seps));
            config->text_header_color = atoi(strtok(NULL, seps));
            // printf("%s\n", config->text_header_color);
        }
        else if(strcmp(fragmented_line, "odd_background_line_color") == 0){
            // strcpy(config->odd_background_line_color, strtok(NULL, seps));
            config->odd_background_line_color = atoi(strtok(NULL, seps));
            // printf("%s\n", config->odd_background_line_color);
        }
        else if(strcmp(fragmented_line, "odd_text_line_color") == 0){
            // strcpy(config->odd_text_line_color, strtok(NULL, seps));
            config->odd_text_line_color = atoi(strtok(NULL, seps));
            // printf("%s\n", config->odd_text_line_color);
        }
        else if(strcmp(fragmented_line, "even_background_line_color") == 0){
            // strcpy(config->even_background_line_color, strtok(NULL, seps));
            config->even_background_line_color = atoi(strtok(NULL, seps));
            // printf("%s\n", config->even_background_line_color);
        }
        else if(strcmp(fragmented_line, "even_text_line_color") == 0){
            // strcpy(config->even_text_line_color, strtok(NULL, seps));
            config->even_text_line_color = atoi(strtok(NULL, seps));
            // printf("%s\n", config->even_text_line_color);
        }
        else if(strcmp(fragmented_line, "selection_background_color") == 0){
            // strcpy(config->selection_background_color, strtok(NULL, seps));
            config->selection_background_color = atoi(strtok(NULL, seps));
            // printf("%s\n", config->selection_background_color);
        }
        else if(strcmp(fragmented_line, "selection_text_color") == 0){
            // strcpy(config->selection_text_color, strtok(NULL, seps));
            config->selection_text_color = atoi(strtok(NULL, seps));
            // printf("%s\n", config->selection_text_color);
        }
        else if(strcmp(fragmented_line, "table_type") == 0){
            // strcpy(config->table_type, strtok(NULL, seps));
            fragmented_line = strtok(NULL, seps);
            if(strcmp(fragmented_line, "static") == 0)
                config->table_type = 0;
            else if(strcmp(fragmented_line, "dynamic") == 0)
                config->table_type = 1;
            else
                config->table_type = 0;
            // printf("%s\n", config->table_type);
        }
        else if(strcmp(fragmented_line, "file_line_max_length") == 0){
            // strcpy(config->file_line_max_length, strtok(NULL, seps));
            config->file_line_max_length = atoi(strtok(NULL, seps));
            // printf("%s\n", config->file_line_max_length);
        }
        else if(strcmp(fragmented_line, "file_max_length") == 0){
            // strcpy(config->file_max_length, strtok(NULL, seps));
            config->file_max_length = atoi(strtok(NULL, seps));
            // printf("%s\n", config->file_max_length);
        }
        else if(strcmp(fragmented_line, "unfocused_cell_max_width") == 0){
            // strcpy(config->unfocused_cell_max_width, strtok(NULL, seps));
            config->unfocused_cell_max_width = atoi(strtok(NULL, seps));
            // printf("%s\n", config->unfocused_cell_max_width);
        }
        else if(strcmp(fragmented_line, "focused_cell_max_width") == 0){
            // strcpy(config->focused_cell_max_width, strtok(NULL, seps));
            config->focused_cell_max_width = atoi(strtok(NULL, seps));
            // printf("%s\n", config->focused_cell_max_width);
        }
        else if(strcmp(fragmented_line, "cell_max_width") == 0){
            // strcpy(config->cell_max_width, strtok(NULL, seps));
            config->cell_max_width = atoi(strtok(NULL, seps));
            // printf("%s\n", config->cell_max_width);
        }
        else if(strcmp(fragmented_line, "csv_default_separator") == 0){
            config->input_separator = strtok(NULL, seps)[0];
            config->output_separator = config->input_separator;
            // printf("%c\n", config->separator);
        }
        else if(strcmp(fragmented_line, "commands_history_length") == 0){
            // strcpy(config->commands_history_length, strtok(NULL, seps));
            config->commands_history_length = atoi(strtok(NULL, seps));
            // printf("%s\n", config->commands_history_length);
        }
    }

    #ifdef __linux__
    init_pair(1, config->default_text_color / 10 < 4 ? config->default_text_color%10 : 8 + config->default_text_color%10, config->default_background_color / 10 < 5 ? config->default_background_color%10 : 8 + config->default_background_color%10);
    init_pair(2, config->default_text_color / 10 < 4 ? config->default_text_color%10 : 8 + config->default_text_color%10, 0);
    init_pair(3, config->text_header_color / 10 < 4 ? config->text_header_color%10 : 8 + config->text_header_color%10, config->background_header_color / 10 < 5 ? config->background_header_color%10 : 8 + config->background_header_color%10);
    init_pair(4, config->odd_text_line_color / 10 < 4 ? config->odd_text_line_color%10 : 8 + config->odd_text_line_color%10, config->odd_background_line_color / 10 < 5 ? config->odd_background_line_color%10 : 8 + config->odd_background_line_color%10);
    init_pair(5, config->even_text_line_color / 10 < 4 ? config->even_text_line_color%10 : 8 + config->even_text_line_color%10, config->even_background_line_color / 10 < 5 ? config->even_background_line_color%10 : 8 + config->even_background_line_color%10);
    init_pair(6, config->selection_text_color / 10 < 4 ? config->selection_text_color%10 : 8 + config->selection_text_color%10, config->selection_background_color / 10 < 5 ? config->selection_background_color%10 : 8 + config->selection_background_color%10);
    #endif

    fclose(f);
}

void Get_Config_From_Args(config_type config, int argc, char **argv){
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "--input") == 0 || strcmp(argv[i], "-i") == 0){
            if(i + 1 < argc && argv[i + 1][0] != "-"){
                strcpy(config->input_file, argv[i + 1]);
                i++;
            }
        }
        else if(strcmp(argv[i], "--separator") == 0 || strcmp(argv[i], "-sep") == 0){
            if(i + 1 < argc && argv[i + 1][0] != "-"){
                config->input_separator = argv[i + 1][0];
                config->output_separator = config->input_separator;
                i++;
            }
        }
        else if(strcmp(argv[i], "--show-config") == 0){
            config->show_config = 1;
        }
    }
}

#ifdef __linux__
void Get_Window_Size(int *columns, int *rows){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *rows = w.ws_row;
    *columns = w.ws_col;
}
#endif

#ifdef _WIN64
void Get_Window_Size(int *columns, int *rows){
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
#endif

void Set_Window_Size(config_type config){
    int columns, rows;

    Get_Window_Size(&columns, &rows);

    // printf("columns: %d\n", columns);
    // printf("rows: %d\n", rows);

    config->window_width = columns;
    config->window_length = rows;
}

void Set_Max_Table_Length(config_type config){
    if(config->table_type == 0)
        config->max_table_length = -1;
    else if(config->table_type == 1)
        config->max_table_length = config->window_length + 100;
    else
        config->max_table_length = -1;
}

void Print_Config(config_type config){
    no_output_string_display("Configuration :\n");
    no_output_string_display("General :\n");
    no_output_string_display("\tInstallation directory : %s\n", config->installation_directory);
    no_output_string_display("Colors :\n");
    no_output_string_display("\tDefault background color : %d\n", config->default_background_color);
    no_output_string_display("\tDefault text color : %d\n", config->default_text_color);
    no_output_string_display("\tText border color : %d\n", config->text_border_color);
    no_output_string_display("\tBackground header color : %d\n", config->background_header_color);
    no_output_string_display("\tText header color : %d\n", config->text_header_color);
    no_output_string_display("\tOdd background line color : %d\n", config->odd_background_line_color);
    no_output_string_display("\tOdd text line color : %d\n", config->odd_text_line_color);
    no_output_string_display("\tEven background line color : %d\n", config->even_background_line_color);
    no_output_string_display("\tEven text line color : %d\n", config->even_text_line_color);
    no_output_string_display("\tSelection background color : %d\n", config->selection_background_color);
    no_output_string_display("\tSelection text color : %d\n", config->selection_text_color);
    no_output_string_display("Table :\n");
    if(config->table_type == 0)
        no_output_string_display("\tTable type : static\n");
    else if(config->table_type == 1)
        no_output_string_display("\tTable type : dynamic\n");
    else
        no_output_string_display("\tTable type : UNRECOGNIZED\n");
    no_output_string_display("\tFile line max length : %d\n", config->file_line_max_length);
    no_output_string_display("\tFile max length : %d\n", config->file_max_length);
    no_output_string_display("\tUnfocused cell max width : %d\n", config->unfocused_cell_max_width);
    no_output_string_display("\tFocused cell max width : %d\n", config->focused_cell_max_width);
    no_output_string_display("\tCell max width : %d\n", config->cell_max_width);
    no_output_string_display("\tTable length : %d\n", config->max_table_length);
    no_output_string_display("Session config :\n");
    no_output_string_display("\tInput file : %s\n", config->input_file);
    no_output_string_display("\tInput separator : %c\n", config->input_separator);
    no_output_string_display("\tOutput separator : %c\n", config->output_separator);
    no_output_string_display("Window specifications :\n");
    no_output_string_display("\tWindow width : %d\n", config->window_width);
    no_output_string_display("\tWindow length : %d\n", config->window_length);
    no_output_string_display("Other :\n");
    no_output_string_display("\tCommands history length : %d\n", config->commands_history_length);
    #ifdef __linux__
    nodelay(stdscr, FALSE);
    getch();
    nodelay(stdscr, TRUE);
    #endif
}