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
    Get_Window_Size(config);
    Set_Max_Table_Length(config);
    // Print_Config(config);
    table_type table_object = malloc(sizeof(*table_object));
    Fetch_Data_From_Csv(table_object, config, 0, -1, -1, -1);
    table_object->first_line_printed = 0;
    table_object->fixed_column = 0;
    table_object->active_line = -2;
    table_object->active_column = -1;
    table_object->first_character_printed = -1;
    table_object->character_highlighted = -1;
    Print_Table(table_object, config);
    int running = 1;
    char c;
    int state = REGULAR_STATE;
    int buffer = 0;


    while(running){
        Get_Window_Size(config);
        // Clear_Screen();
        // Print_Config(config);
        Print_Table(table_object, config);
        // _sleep(100);
        for(;;){
            if(kbhit()){
                c = getch();
                if(c == -32){
                    c = getch();
                    if(c == 72){    // Up
                        if(state == REGULAR_STATE && table_object->active_line > -2){
                            table_object->active_line--;
                            break;
                        }
                        else if(state == MOVE_STATE && table_object->active_column == -1 && table_object->active_line > -1){

                        }
                    }
                    else if(c == 80){    // Down
                        if(state == REGULAR_STATE && table_object->active_line < table_object->table_length - 1){
                            table_object->active_line++;
                            break;
                        }
                        else if(state == MOVE_STATE && table_object->active_column == -1 && table_object->active_line > -1){

                        }
                    }
                    else if(c == 75){    // Left
                        if(state == REGULAR_STATE && table_object->active_column > -1){
                            table_object->active_column--;
                            break;
                        }
                        else if(state == MOVE_STATE && table_object->active_column > -1 && table_object->active_line == -2){
                            if(table_object->active_column > 0){
                                buffer = table_object->columns_order_of_display[table_object->active_column - 1];
                                table_object->columns_order_of_display[table_object->active_column - 1] = table_object->columns_order_of_display[table_object->active_column];
                                table_object->columns_order_of_display[table_object->active_column] = buffer;
                                table_object->active_column--;
                                break;
                            }
                        }
                    }
                    else if(c == 77){    // Right
                        if(state == REGULAR_STATE && table_object->active_column < table_object->table_width - 1){
                            table_object->active_column++;
                            break;
                        }
                        else if(state == MOVE_STATE && table_object->active_column > -1 && table_object->active_line == -2){
                            if(table_object->active_column < table_object->table_width - 1){
                                buffer = table_object->columns_order_of_display[table_object->active_column + 1];
                                table_object->columns_order_of_display[table_object->active_column + 1] = table_object->columns_order_of_display[table_object->active_column];
                                table_object->columns_order_of_display[table_object->active_column] = buffer;
                                table_object->active_column++;
                                break;
                            }
                        }
                    }
                }
                else if(c == 'm'){
                    if(state == REGULAR_STATE)
                        state = MOVE_STATE;
                    else if(state == MOVE_STATE)
                        state = REGULAR_STATE;
                }
                else if(c == 27){    // ESC
                    running = 0;
                    break;
                }
            }
        }
    }


    table_object = Free_Table_Object(table_object);
    free(config);   // Push to right place, useless here
    Clear_Screen();
    return 0;
}