#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libloaderapi.h>
// #include <sys/ioctl.h>
// #include <winioctl.h>
#include <windows.h>
#include <conio.h>
#include "../lib/config.h"
#include "../lib/colors.h"
#include "../lib/table.h"
#include "../lib/constants.h"

int main(int argc, char **argv){
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

        Clear_Screen_By_Scrolldown();

        while(running){
            Set_Window_Size(config);
            // Clear_Screen_By_Scrolldown();
            // Print_Config(config);
            S_Print_Table(table_object, config);
            // _sleep(100);
            for(;;){
                if(kbhit()){
                    c = getch();
                    if(state == REGULAR_STATE){
                        if(c == 'm'){
                            state = MOVE_STATE;
                        }
                        else if(c == 'i' && table_object->active_column > -1 && table_object->active_line > -2){
                            if(table_object->active_line == -1){
                                table_object->character_highlighted = strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]);
                            }
                            else{
                                table_object->character_highlighted = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                            }
                            state = EDIT_STATE;
                            break;
                        }
                        else if(c == 27){
                            state = COMMAND_STATE;
                            table_object->command_character_highlighted = 0;
                            break;
                        }
                        else if(c == -32){
                            c = getch();
                            if(c == 72){    // Up
                                if(table_object->active_line > -2){   // Go up
                                    table_object->active_line--;
                                    break;
                                }
                            }
                            else if(c == 80){   // Down
                                if(table_object->active_line < table_object->table_length - 1){   // Go down
                                    table_object->active_line++;
                                    break;
                                }
                            }
                            else if(c == 75){   // Left
                                if(table_object->active_column > -1){ // Go left
                                    table_object->active_column--;
                                    break;
                                }
                            }
                            else if(c == 77){   // Right
                                if(table_object->active_column < table_object->table_width - 1){  // Go right
                                    table_object->active_column++;
                                    break;
                                }
                            }
                        }
                    }
                    else if(state == MOVE_STATE){
                        if(c == 'm' || c == 'r'){
                            state = REGULAR_STATE;
                        }
                        else if(c == 27){
                            state = COMMAND_STATE;
                            table_object->command_character_highlighted = 0;
                            break;
                        }
                        else if(c == -32){
                            c = getch();
                            if(c == 72){    // Up
                                if(table_object->active_column == -1 && table_object->active_line > -1){    // Move line up
                                    if(table_object->active_line > 0){
                                        for(int i = 0; i < table_object->table_width; i++){
                                            strcpy(string_buffer, table_object->table[table_object->active_line - 1][i]);
                                            strcpy(table_object->table[table_object->active_line - 1][i], table_object->table[table_object->active_line][i]);
                                            strcpy(table_object->table[table_object->active_line][i], string_buffer);
                                        }
                                        table_object->active_line--;
                                        break;
                                    }
                                }
                                else if(table_object->active_column > -1 && table_object->active_line > -1){    // Move cell up
                                    if(table_object->active_line > 0){
                                        strcpy(string_buffer, table_object->table[table_object->active_line - 1][table_object->columns_order_of_display[table_object->active_column]]);
                                        strcpy(table_object->table[table_object->active_line - 1][table_object->columns_order_of_display[table_object->active_column]], table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                                        strcpy(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]], string_buffer);
                                        table_object->active_line--;
                                        break;
                                    }
                                }
                            }
                            else if(c == 80){   // Down
                                if(table_object->active_column == -1 && table_object->active_line > -1){    // Move line down
                                    if(table_object->active_line < table_object->table_length - 1){
                                        for(int i = 0; i < table_object->table_width; i++){
                                            strcpy(string_buffer, table_object->table[table_object->active_line + 1][i]);
                                            strcpy(table_object->table[table_object->active_line + 1][i], table_object->table[table_object->active_line][i]);
                                            strcpy(table_object->table[table_object->active_line][i], string_buffer);
                                        }
                                        table_object->active_line++;
                                        break;
                                    }
                                }
                                else if(table_object->active_column > -1 && table_object->active_line > -1){    // Move cell down
                                    if(table_object->active_line < table_object->table_length - 1){
                                        strcpy(string_buffer, table_object->table[table_object->active_line + 1][table_object->columns_order_of_display[table_object->active_column]]);
                                        strcpy(table_object->table[table_object->active_line + 1][table_object->columns_order_of_display[table_object->active_column]], table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                                        strcpy(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]], string_buffer);
                                        table_object->active_line++;
                                        break;
                                    }
                                }
                            }
                            else if(c == 75){   // Left
                                if(table_object->active_column > -1 && table_object->active_line == -2){    // Move column left
                                    if(table_object->active_column > 0){
                                        buffer = table_object->columns_order_of_display[table_object->active_column - 1];
                                        table_object->columns_order_of_display[table_object->active_column - 1] = table_object->columns_order_of_display[table_object->active_column];
                                        table_object->columns_order_of_display[table_object->active_column] = buffer;
                                        table_object->active_column--;
                                        break;
                                    }
                                }
                                else if(table_object->active_column > -1 && table_object->active_line > -1){    // Move cell left
                                    if(table_object->active_column > 0){
                                        strcpy(string_buffer, table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column - 1]]);
                                        strcpy(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column - 1]], table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                                        strcpy(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]], string_buffer);
                                        table_object->active_column--;
                                        if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                            table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                                        }
                                        else if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                            Update_Cell_Width_By_Column(table_object, table_object->active_column);
                                        }
                                        if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column + 1]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column + 1]]){
                                            table_object->cell_width[table_object->columns_order_of_display[table_object->active_column + 1]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column + 1]]);
                                        }
                                        else if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column + 1]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column + 1]]){
                                            Update_Cell_Width_By_Column(table_object, table_object->active_column + 1);
                                        }
                                        break;
                                    }
                                }
                            }
                            else if(c == 77){   // Right
                                if(table_object->active_column > -1 && table_object->active_line == -2){    // Move column right
                                    if(table_object->active_column < table_object->table_width - 1){
                                        buffer = table_object->columns_order_of_display[table_object->active_column + 1];
                                        table_object->columns_order_of_display[table_object->active_column + 1] = table_object->columns_order_of_display[table_object->active_column];
                                        table_object->columns_order_of_display[table_object->active_column] = buffer;
                                        table_object->active_column++;
                                        break;
                                    }
                                }
                                else if(table_object->active_column > -1 && table_object->active_line > -1){    // Move cell right
                                    if(table_object->active_column < table_object->table_width - 1){
                                        strcpy(string_buffer, table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column + 1]]);
                                        strcpy(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column + 1]], table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                                        strcpy(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]], string_buffer);
                                        table_object->active_column++;
                                        if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                            table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                                        }
                                        else if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                            Update_Cell_Width_By_Column(table_object, table_object->active_column);
                                        }
                                        if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column - 1]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column - 1]]){
                                            table_object->cell_width[table_object->columns_order_of_display[table_object->active_column - 1]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column - 1]]);
                                        }
                                        else if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column - 1]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column - 1]]){
                                            Update_Cell_Width_By_Column(table_object, table_object->active_column - 1);
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else if(state == COMMAND_STATE){
                        if(c == 27){    // ESC
                            table_object->command[0][0] = '\0';
                            table_object->command_character_highlighted = -1;
                            table_object->active_command = 0;
                            state = REGULAR_STATE;
                            break;
                        }
                        else if(c == -32){
                            c = getch();
                            if(c == 72){    // Up
                                if(table_object->active_command < config->commands_history_length && strlen(table_object->command[table_object->active_command + 1]) > 0){
                                    table_object->active_command++;
                                    table_object->command_character_highlighted = strlen(table_object->command[table_object->active_command]);
                                    break;
                                }
                            }
                            else if(c == 80){   // Down
                                if(table_object->active_command > 0){
                                    table_object->active_command--;
                                    table_object->command_character_highlighted = strlen(table_object->command[table_object->active_command]);
                                    break;
                                }
                            }
                            else if(c == 75){   // Left
                                if(table_object->command_character_highlighted > 0){
                                    table_object->command_character_highlighted--;
                                    break;
                                }
                            }
                            else if(c == 77){   // Right
                                if(table_object->command_character_highlighted < strlen(table_object->command[table_object->active_command])){
                                    table_object->command_character_highlighted++;
                                    break;
                                }
                            }
                        }
                        else if(c == 13){   // enter
                            if(table_object->active_command != 0){
                                strcpy(table_object->command[0], table_object->command[table_object->active_command]);
                            }
                            if(strcmp(table_object->command[0], ":q") == 0){
                                running = 0;
                            }
                            else if(strstr(table_object->command[0], ":jump ") == table_object->command[0]){
                                command_buffer = strstr(table_object->command[0], ":jump ") + strlen(":jump ");
                                buffer = atoi(command_buffer) - 1;
                                if(buffer < -1){
                                    table_object->active_line = -2;
                                }
                                else if(buffer > table_object->table_length - 1){
                                    table_object->active_line = table_object->table_length - 1;
                                }
                                else{
                                    table_object->active_line = buffer;
                                }
                            }
                            buffer = 0;
                            for(int i = 0; i < config->commands_history_length && strcmp(table_object->command[i], "\0") != 0; i++){
                                buffer = i;
                            }
                            for(int i = buffer + 1; i > 0; i--){
                                strcpy(table_object->command[i], table_object->command[i - 1]);
                            }
                            table_object->command[0][0] = '\0';
                            table_object->command_character_highlighted = -1;
                            table_object->active_command = 0;
                            state = REGULAR_STATE;
                            break;
                        }
                        else if(c == 8){    // backspace
                            // if(strlen(table_object->command) > 0){
                            //     table_object->command[strlen(table_object->command) - 1] = '\0';
                            //     table_object->command_character_highlighted--;
                            //     break;
                            // }
                            if(table_object->active_command != 0){
                                strcpy(table_object->command[0], table_object->command[table_object->active_command]);
                                table_object->active_command = 0;
                            }
                            if(table_object->command_character_highlighted > 0){
                                for(int i = table_object->command_character_highlighted - 1; i < strlen(table_object->command[0]); i++){
                                    table_object->command[0][i] = table_object->command[0][i + 1];
                                }
                                table_object->command_character_highlighted--;
                                break;
                            }
                        }
                        else{   // other characters
                            if(table_object->active_command != 0){
                                strcpy(table_object->command[0], table_object->command[table_object->active_command]);
                                table_object->active_command = 0;
                            }
                            if(strlen(table_object->command[0]) < command_string_size - 1){
                                // table_object->command[strlen(table_object->command) + 1] = '\0';
                                // table_object->command[strlen(table_object->command)] = c;
                                // table_object->command_character_highlighted++;
                                for(int i = strlen(table_object->command[0]); i >= table_object->command_character_highlighted; i--){
                                    table_object->command[0][i + 1] = table_object->command[0][i];
                                }
                                table_object->command[0][table_object->command_character_highlighted] = c;
                                table_object->command_character_highlighted++;
                                break;
                            }
                        }
                    }
                    else if(state == EDIT_STATE){
                        if(c == 27 || c == 13){    // ESC or enter
                            state = REGULAR_STATE;
                            table_object->character_highlighted = -1;
                            table_object->first_character_printed = -1;
                            break;
                        }
                        else if(c == -32){
                            c = getch();
                            if(c == 72){    // Up
                                
                            }
                            else if(c == 80){   // Down
                                
                            }
                            else if(c == 75){   // Left
                                if(table_object->active_line == -1){
                                    if(table_object->character_highlighted > 0){
                                        if(table_object->first_character_printed > 0 && table_object->character_highlighted == strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                                            table_object->first_character_printed--;
                                        }
                                        table_object->character_highlighted--;
                                        break;
                                    }
                                }
                                else{
                                    if(table_object->character_highlighted > 0){
                                        if(table_object->first_character_printed > 0 && table_object->character_highlighted == strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]])){
                                            table_object->first_character_printed--;
                                        }
                                        table_object->character_highlighted--;
                                        break;
                                    }
                                }
                            }
                            else if(c == 77){   // Right
                                if(table_object->active_line == -1){
                                    if(table_object->character_highlighted < strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                                        table_object->character_highlighted++;
                                        break;
                                    }
                                }
                                else{
                                    if(table_object->character_highlighted < strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]])){
                                        table_object->character_highlighted++;
                                        break;
                                    }
                                }
                            }
                        }
                        else if(c == 8){    // backspace
                            if(table_object->active_column > -1 && table_object->active_line == -1){
                                // if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) > 0){
                                //     table_object->header[table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) - 1] = '\0';
                                //     if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                //         table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]);
                                //     }
                                //     else if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                //         Update_Cell_Width_By_Column(table_object, table_object->active_column);
                                //     }
                                //     table_object->character_highlighted--;
                                //     break;
                                // }
                                if(table_object->character_highlighted > 0){
                                    // table_object->header[table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) - 1] = '\0';
                                    for(int i = table_object->character_highlighted - 1; i < strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]); i++){
                                        table_object->header[table_object->columns_order_of_display[table_object->active_column]][i] = table_object->header[table_object->columns_order_of_display[table_object->active_column]][i + 1];
                                    }
                                    if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                        table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]);
                                    }
                                    else if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                        Update_Cell_Width_By_Column(table_object, table_object->active_column);
                                    }
                                    table_object->character_highlighted--;
                                    break;
                                }
                            }
                            else if(table_object->active_column > -1 && table_object->active_line > -1){
                                // if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > 0){
                                //     table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) - 1] = '\0';
                                //     if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                //         table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                                //     }
                                //     else if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                //         Update_Cell_Width_By_Column(table_object, table_object->active_column);
                                //     }
                                //     table_object->character_highlighted--;
                                //     break;
                                // }
                                if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > 0){
                                    // table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) - 1] = '\0';
                                    for(int i = table_object->character_highlighted - 1; i < strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]); i++){
                                        table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][i] = table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][i + 1];
                                    }
                                    if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                        table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                                    }
                                    else if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                        Update_Cell_Width_By_Column(table_object, table_object->active_column);
                                    }
                                    table_object->character_highlighted--;
                                    break;
                                }
                            }
                            // if(strlen(table_object->command) > 0){
                            //     table_object->command[strlen(table_object->command) - 1] = '\0';
                            //     break;
                            // }
                        }
                        else{
                            if(table_object->active_column > -1 && table_object->active_line == -1){
                                // if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) < config->cell_max_width + 1){
                                //     table_object->header[table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) + 1] = '\0';
                                //     table_object->header[table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])] = c;
                                //     if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                //         table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]);
                                //     }
                                //     else if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                //         Update_Cell_Width_By_Column(table_object, table_object->active_column);
                                //     }
                                //     table_object->character_highlighted++;
                                //     break;
                                // }
                                if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) < config->cell_max_width + 1){
                                    // table_object->header[table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) + 1] = '\0';
                                    // table_object->header[table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])] = c;
                                    for(int i = strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]); i >= table_object->character_highlighted; i--){
                                        table_object->header[table_object->columns_order_of_display[table_object->active_column]][i + 1] = table_object->header[table_object->columns_order_of_display[table_object->active_column]][i];
                                    }
                                    table_object->header[table_object->columns_order_of_display[table_object->active_column]][table_object->character_highlighted] = c;
                                    if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                        table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]);
                                    }
                                    else if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                        Update_Cell_Width_By_Column(table_object, table_object->active_column);
                                    }
                                    table_object->character_highlighted++;
                                    break;
                                }
                            }
                            else if(table_object->active_column > -1 && table_object->active_line > -1){
                                // if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < config->cell_max_width + 1){
                                //     table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) + 1] = '\0';
                                //     table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]])] = c;
                                //     if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                //         table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                                //     }
                                //     else if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                //         Update_Cell_Width_By_Column(table_object, table_object->active_column);
                                //     }
                                //     table_object->character_highlighted++;
                                //     break;
                                // }
                                if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < config->cell_max_width + 1){
                                    // table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) + 1] = '\0';
                                    // table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]])] = c;
                                    for(int i = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]); i >= table_object->character_highlighted; i--){
                                        table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][i + 1] = table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][i];
                                    }
                                    table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][table_object->character_highlighted] = c;
                                    if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                        table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                                    }
                                    else if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                                        Update_Cell_Width_By_Column(table_object, table_object->active_column);
                                    }
                                    table_object->character_highlighted++;
                                    break;
                                }
                            }
                            // if(strlen(table_object->command) < command_string_size - 1){
                            //     table_object->command[strlen(table_object->command) + 1] = '\0';
                            //     table_object->command[strlen(table_object->command)] = c;
                            //     break;
                            // }
                        }
                    }
                }
                Get_Window_Size(&window_columns_buffer, &window_rows_buffer);
                if(config->window_length != window_rows_buffer || config->window_width != window_columns_buffer){
                    Set_Window_Size(config);
                    break;
                }
            }
        }
        
        table_object = Free_Table_Object(table_object, config);
        free(config);   // Push to right place, useless here
        free(string_buffer);

        Clear_Screen_By_Cleaning();
    }
    return 0;
}