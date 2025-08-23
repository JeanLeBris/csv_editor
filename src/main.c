/* ifeq ($(uname -s),Linux)
	LIB_TO_ADD="-lncurses "
	$(info	Compile to Linux)
	@echo Compile to Linux
	compile:csv_editor.exe
endif */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include <sys/ioctl.h>
// #include <winioctl.h>
#ifdef __linux__
#include <ncurses.h>
#endif
#ifdef _WIN64
#include <libloaderapi.h>
#include <windows.h>
#include <conio.h>
#endif
#include "../lib/config.h"
#include "../lib/colors.h"
#include "../lib/table.h"
#include "../lib/actions.h"
#include "../lib/constants.h"

int main(int argc, char **argv){
    #ifdef __linux__
    initscr();
    start_color();
    //cbreak();
    noecho();
    //scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    #endif
    config_type config = malloc(sizeof(*config));
    Set_Default_Config(config, argv[0]);
    Load_Config(config);
    Get_Config_From_Args(config, argc, argv);
    Set_Window_Size(config);
    Set_Max_Table_Length(config);
    if(config->show_config){
        Print_Config(config);
    }
    else{
        table_type table_object = malloc(sizeof(*table_object));
        Fetch_Data_From_Csv(table_object, config, 0, -1, -1, -1);
        table_object->first_line_printed = 0;
        table_object->first_column_printed = 0;
        table_object->active_line = -2;
        table_object->active_column = -1;
        table_object->first_character_printed = -1;
        table_object->character_highlighted = -1;
        // table_object->command[0] = '\0';
        // S_Print_Table(table_object, config);
        int running = 1;
        char c;
        int state = REGULAR_STATE;
        int buffer = 0;
        int window_columns_buffer = 0;
        int window_rows_buffer = 0;
        char *string_buffer = malloc((config->cell_max_width + 1) * sizeof(char));
        int command_string_size = 101;
        table_object->command = malloc((config->commands_history_length + 1) * sizeof(char*));
        for(int i = 0; i < config->commands_history_length + 1; i++){
            table_object->command[i] = malloc(command_string_size * sizeof(char));
            table_object->command[i][0] = '\0';
        }
        char *command_buffer = NULL;
        table_object->command_character_highlighted = -1;
        table_object->active_command = 0;

        #ifdef __linux__
        char stdin_buffer[20] = "\0";
        int stdin_count = 0;
        char stdin_char = '\0';
        #endif

        Clear_Screen_By_Scrolldown();

        int refresh_screen_bool = 1;
        while(running){
            Set_Window_Size(config);
            // Clear_Screen_By_Scrolldown();
            // Print_Config(config);
            Print_Table(table_object, config, state);
            // _sleep(100);
            refresh_screen_bool = 0;
            while(refresh_screen_bool == 0){
                #ifdef __linux__
                if(1){
                    while((stdin_char = getch()) == -1){
                        usleep(10000); // 10ms delay
                    }
                    stdin_buffer[0] = stdin_char;
                    stdin_count = 1;
                    while((stdin_char = getch()) != -1){
                        stdin_buffer[stdin_count] = stdin_char;
                        stdin_count++;
                    }
                    stdin_buffer[stdin_count] = '\0';
                    //c = getch();
                    if(state == REGULAR_STATE){
                        if(strlen(stdin_buffer) == 1 && stdin_buffer[0] == 'm'){
                            state = MOVE_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(strlen(stdin_buffer) == 1 && stdin_buffer[0] == 'i' && table_object->active_column > -1 && table_object->active_line > -2){
                            set_character_highlighted_to_last_character(table_object);
                            state = EDIT_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(strlen(stdin_buffer) == 1 && stdin_buffer[0] == 27){
                            state = COMMAND_STATE;
                            table_object->command_character_highlighted = 0;
                            refresh_screen_bool = 1;
                        }
                        else if(strlen(stdin_buffer) >= 3 && stdin_buffer[0] == 27 && stdin_buffer[1] == 91){
                            //c = getch();
                            if(stdin_buffer[strlen(stdin_buffer) - 1] == 65){    // Up
                                refresh_screen_bool = on_selection_go_up(table_object);
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 66){   // Down
                                refresh_screen_bool = on_selection_go_down(table_object);
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 68){   // Left
                                refresh_screen_bool = on_selection_go_left(table_object);
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 67){   // Right
                                refresh_screen_bool = on_selection_go_right(table_object);
                            }
                        }
                    }
                    else if(state == MOVE_STATE){
                        if(strlen(stdin_buffer) == 1 && (stdin_buffer[0] == 'm' || stdin_buffer[0] == 'r')){
                            state = REGULAR_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(strlen(stdin_buffer) == 1 && stdin_buffer[0] == 27){
                            state = COMMAND_STATE;
                            table_object->command_character_highlighted = 0;
                            refresh_screen_bool = 1;
                        }
                        else if(strlen(stdin_buffer) >= 3 && stdin_buffer[0] == 27 && stdin_buffer[1] == 91){
                            if(stdin_buffer[strlen(stdin_buffer) - 1] == 65){    // Up
                                refresh_screen_bool = on_move_go_up(table_object, string_buffer);
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 66){   // Down
                                refresh_screen_bool = on_move_go_down(table_object, string_buffer);
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 68){   // Left
                                refresh_screen_bool = on_move_go_left(table_object, string_buffer);
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 67){   // Right
                                refresh_screen_bool = on_move_go_right(table_object, string_buffer);
                            }
                        }
                    }
                    else if(state == COMMAND_STATE){
                        if(strlen(stdin_buffer) == 1 && stdin_buffer[0] == 27){    // ESC
                            delete_command(table_object);
                            state = REGULAR_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(strlen(stdin_buffer) >= 3 && stdin_buffer[0] == 27 && stdin_buffer[1] == 91){
                            if(stdin_buffer[strlen(stdin_buffer) - 1] == 65){    // Up
                                refresh_screen_bool = on_command_go_up(table_object, config->commands_history_length);
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 66){   // Down
                                refresh_screen_bool = on_command_go_down(table_object);
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 68){   // Left
                                refresh_screen_bool = on_command_go_left(table_object);
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 67){   // Right
                                refresh_screen_bool = on_command_go_right(table_object);
                            }
                        }
                        else if(strlen(stdin_buffer) == 1 && stdin_buffer[0] == 10){   // enter
                            running = execute_command(table_object, config, config->commands_history_length);
                            state = REGULAR_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(strlen(stdin_buffer) == 1 && stdin_buffer[0] == 127){    // backspace
                            refresh_screen_bool = on_command_backspace(table_object);
                        }
                        else{
                            refresh_screen_bool = on_command_characters(table_object, command_string_size, stdin_buffer[0]);
                        }
                    }
                    else if(state == EDIT_STATE){
                        if(strlen(stdin_buffer) == 1 && (stdin_buffer[0] == 27 || stdin_buffer[0] == 10)){    // ESC or enter
                            reset_character_highlighted_to_default_value(table_object);
                            state = REGULAR_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(strlen(stdin_buffer) >= 3 && stdin_buffer[0] == 27 && stdin_buffer[1] == 91){
                            //c = getch();
                            if(stdin_buffer[strlen(stdin_buffer) - 1] == 65){    // Up
                                
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 66){   // Down
                                
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 68){   // Left
                                refresh_screen_bool = on_edit_go_left(table_object);
                            }
                            else if(stdin_buffer[strlen(stdin_buffer) - 1] == 67){   // Right
                                refresh_screen_bool = on_edit_go_right(table_object);
                            }
                        }
                        else if(strlen(stdin_buffer) == 1 && stdin_buffer[0] == 127){    // backspace
                            refresh_screen_bool = on_edit_backspace(table_object);
                        }
                        else{
                            refresh_screen_bool = on_edit_characters(table_object, config->cell_max_width, stdin_buffer[0]);
                        }
                    }
                }
                #endif
                #ifdef _WIN64
                if(kbhit()){
                    c = getch();
                    if(state == REGULAR_STATE){
                        if(c == 'm'){
                            state = MOVE_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(c == 'i' && table_object->active_column > -1 && table_object->active_line > -2){
                            set_character_highlighted_to_last_character(table_object);
                            state = EDIT_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(c == 27){
                            state = COMMAND_STATE;
                            table_object->command_character_highlighted = 0;
                            refresh_screen_bool = 1;
                        }
                        else if(c == -32 || c == 0){
                            c = getch();
                            if(c == 72){    // Up
                                refresh_screen_bool = on_selection_go_up(table_object);
                            }
                            else if(c == 80){   // Down
                                refresh_screen_bool = on_selection_go_down(table_object);
                            }
                            else if(c == 75){   // Left
                                refresh_screen_bool = on_selection_go_left(table_object);
                            }
                            else if(c == 77){   // Right
                                refresh_screen_bool = on_selection_go_right(table_object);
                            }
                        }
                    }
                    else if(state == MOVE_STATE){
                        if(c == 'm' || c == 'r'){
                            state = REGULAR_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(c == 27){
                            state = COMMAND_STATE;
                            table_object->command_character_highlighted = 0;
                            refresh_screen_bool = 1;
                        }
                        else if(c == -32 || c == 0){
                            c = getch();
                            if(c == 72){    // Up
                                refresh_screen_bool = on_move_go_up(table_object, string_buffer);
                            }
                            else if(c == 80){   // Down
                                refresh_screen_bool = on_move_go_down(table_object, string_buffer);
                            }
                            else if(c == 75){   // Left
                                refresh_screen_bool = on_move_go_left(table_object, string_buffer);
                            }
                            else if(c == 77){   // Right
                                refresh_screen_bool = on_move_go_right(table_object, string_buffer);
                            }
                        }
                    }
                    else if(state == COMMAND_STATE){
                        if(c == 27){    // ESC
                            delete_command(table_object);
                            state = REGULAR_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(c == -32 || c == 0){
                            c = getch();
                            if(c == 72){    // Up
                                refresh_screen_bool = on_command_go_up(table_object, config->commands_history_length);
                            }
                            else if(c == 80){   // Down
                                refresh_screen_bool = on_command_go_down(table_object);
                            }
                            else if(c == 75){   // Left
                                refresh_screen_bool = on_command_go_left(table_object);
                            }
                            else if(c == 77){   // Right
                                refresh_screen_bool = on_command_go_right(table_object);
                            }
                        }
                        else if(c == 13){   // enter
                            running = execute_command(table_object, config, config->commands_history_length);
                            state = REGULAR_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(c == 8){    // backspace
                            refresh_screen_bool = on_command_backspace(table_object);
                        }
                        else{   // other characters
                            refresh_screen_bool = on_command_characters(table_object, command_string_size, c);
                        }
                    }
                    else if(state == EDIT_STATE){
                        if(c == 27 || c == 13){    // ESC or enter
                            reset_character_highlighted_to_default_value(table_object);
                            state = REGULAR_STATE;
                            refresh_screen_bool = 1;
                        }
                        else if(c == -32 || c == 0){
                            c = getch();
                            if(c == 72){    // Up
                                
                            }
                            else if(c == 80){   // Down
                                
                            }
                            else if(c == 75){   // Left
                                refresh_screen_bool = on_edit_go_left(table_object);
                            }
                            else if(c == 77){   // Right
                                refresh_screen_bool = on_edit_go_right(table_object);
                            }
                        }
                        else if(c == 8){    // backspace
                            refresh_screen_bool = on_edit_backspace(table_object);
                        }
                        else{   // other characters
                            refresh_screen_bool = on_edit_characters(table_object, config->cell_max_width, c);
                        }
                    }
                }
                #endif
                Get_Window_Size(&window_columns_buffer, &window_rows_buffer);
                if(config->window_length != window_rows_buffer || config->window_width != window_columns_buffer){
                    Set_Window_Size(config);
                    refresh_screen_bool = 1;
                }
            }
        }
        
        table_object = Free_Table_Object(table_object, config);
        free(config);   // Push to right place, useless here
        free(string_buffer);

        Clear_Screen_By_Cleaning();
    }
    #ifdef __linux__
    endwin();
    #endif
    #ifdef _WIN64
    char reset_cursor_string[10] = "";
    Show_Cursor(reset_cursor_string);
    printf("%s", reset_cursor_string);
    #endif
    return 0;
}