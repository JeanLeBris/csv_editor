#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/actions.h"

void set_character_highlighted_to_last_character(table_type table_object){
    if(table_object->active_line == -1){
        table_object->character_highlighted = strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]);
    }
    else{
        table_object->character_highlighted = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
    }
}

void reset_character_highlighted_to_default_value(table_type table_object){
    table_object->character_highlighted = -1;
    table_object->first_character_printed = -1;
}

int on_selection_go_up(table_type table_object){
    if(table_object->active_line > -2){   // Go up
        table_object->active_line--;
        return 1;
    }
    return 0;
}

int on_selection_go_down(table_type table_object){
    if(table_object->active_line < table_object->table_length - 1){   // Go down
        table_object->active_line++;
        return 1;
    }
    return 0;
}

int on_selection_go_left(table_type table_object){
    if(table_object->active_column > -1){ // Go left
        table_object->active_column--;
        return 1;
    }
    return 0;
}

int on_selection_go_right(table_type table_object){
    if(table_object->active_column < table_object->table_width - 1){  // Go right
        table_object->active_column++;
        return 1;
    }
    return 0;
}

int on_move_go_up(table_type table_object, char *string_buffer){
    if(table_object->active_column == -1 && table_object->active_line > -1){    // Move line up
        if(table_object->active_line > 0){
            for(int i = 0; i < table_object->table_width; i++){
                strcpy(string_buffer, table_object->table[table_object->active_line - 1][i]);
                strcpy(table_object->table[table_object->active_line - 1][i], table_object->table[table_object->active_line][i]);
                strcpy(table_object->table[table_object->active_line][i], string_buffer);
            }
            table_object->active_line--;
            return 1;
        }
    }
    else if(table_object->active_column > -1 && table_object->active_line > -1){    // Move cell up
        if(table_object->active_line > 0){
            strcpy(string_buffer, table_object->table[table_object->active_line - 1][table_object->columns_order_of_display[table_object->active_column]]);
            strcpy(table_object->table[table_object->active_line - 1][table_object->columns_order_of_display[table_object->active_column]], table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
            strcpy(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]], string_buffer);
            table_object->active_line--;
            return 1;
        }
    }
    return 0;
}

int on_move_go_down(table_type table_object, char *string_buffer){
    if(table_object->active_column == -1 && table_object->active_line > -1){    // Move line down
        if(table_object->active_line < table_object->table_length - 1){
            for(int i = 0; i < table_object->table_width; i++){
                strcpy(string_buffer, table_object->table[table_object->active_line + 1][i]);
                strcpy(table_object->table[table_object->active_line + 1][i], table_object->table[table_object->active_line][i]);
                strcpy(table_object->table[table_object->active_line][i], string_buffer);
            }
            table_object->active_line++;
            return 1;
        }
    }
    else if(table_object->active_column > -1 && table_object->active_line > -1){    // Move cell down
        if(table_object->active_line < table_object->table_length - 1){
            strcpy(string_buffer, table_object->table[table_object->active_line + 1][table_object->columns_order_of_display[table_object->active_column]]);
            strcpy(table_object->table[table_object->active_line + 1][table_object->columns_order_of_display[table_object->active_column]], table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
            strcpy(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]], string_buffer);
            table_object->active_line++;
            return 1;
        }
    }
    return 0;
}

int on_move_go_left(table_type table_object, char *string_buffer){
    int buffer = 0;
    if(table_object->active_column > -1 && table_object->active_line == -2){    // Move column left
        if(table_object->active_column > 0){
            buffer = table_object->columns_order_of_display[table_object->active_column - 1];
            table_object->columns_order_of_display[table_object->active_column - 1] = table_object->columns_order_of_display[table_object->active_column];
            table_object->columns_order_of_display[table_object->active_column] = buffer;
            table_object->active_column--;
            return 1;
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
            return 1;
        }
    }
    return 0;
}

int on_move_go_right(table_type table_object, char *string_buffer){
    int buffer = 0;
    if(table_object->active_column > -1 && table_object->active_line == -2){    // Move column right
        if(table_object->active_column < table_object->table_width - 1){
            buffer = table_object->columns_order_of_display[table_object->active_column + 1];
            table_object->columns_order_of_display[table_object->active_column + 1] = table_object->columns_order_of_display[table_object->active_column];
            table_object->columns_order_of_display[table_object->active_column] = buffer;
            table_object->active_column++;
            return 1;
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
            return 1;
        }
    }
    return 0;
}

void delete_command(table_type table_object){
    table_object->command[0][0] = '\0';
    table_object->command_character_highlighted = -1;
    table_object->active_command = 0;
}

int on_command_go_up(table_type table_object, int commands_history_length){
    if(table_object->active_command < commands_history_length && strlen(table_object->command[table_object->active_command + 1]) > 0){
        table_object->active_command++;
        table_object->command_character_highlighted = strlen(table_object->command[table_object->active_command]);
        return 1;
    }
    return 0;
}

int on_command_go_down(table_type table_object){
    if(table_object->active_command > 0){
        table_object->active_command--;
        table_object->command_character_highlighted = strlen(table_object->command[table_object->active_command]);
        return 1;
    }
    return 0;
}

int on_command_go_left(table_type table_object){
    if(table_object->command_character_highlighted > 0){
        table_object->command_character_highlighted--;
        return 1;
    }
    return 0;
}

int on_command_go_right(table_type table_object){
    if(table_object->command_character_highlighted < strlen(table_object->command[table_object->active_command])){
        table_object->command_character_highlighted++;
        return 1;
    }
    return 0;
}

int execute_command(table_type table_object, int commands_history_length){
    int buffer = 0;
    int running = 1;
    char *command_buffer = NULL;
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
    for(int i = 0; i < commands_history_length && strcmp(table_object->command[i], "\0") != 0; i++){
        buffer = i;
    }
    for(int i = buffer + 1; i > 0; i--){
        strcpy(table_object->command[i], table_object->command[i - 1]);
    }
    table_object->command[0][0] = '\0';
    table_object->command_character_highlighted = -1;
    table_object->active_command = 0;
    return running;
}

int on_command_backspace(table_type table_object){
    if(table_object->active_command != 0){
        strcpy(table_object->command[0], table_object->command[table_object->active_command]);
        table_object->active_command = 0;
    }
    if(table_object->command_character_highlighted > 0){
        for(int i = table_object->command_character_highlighted - 1; i < strlen(table_object->command[0]); i++){
            table_object->command[0][i] = table_object->command[0][i + 1];
        }
        table_object->command_character_highlighted--;
        return 1;
    }
    return 0;
}

int on_command_characters(table_type table_object, int command_string_size, char c){
    if(table_object->active_command != 0){
        strcpy(table_object->command[0], table_object->command[table_object->active_command]);
        table_object->active_command = 0;
    }
    if(strlen(table_object->command[0]) < command_string_size - 1){
        for(int i = strlen(table_object->command[0]); i >= table_object->command_character_highlighted; i--){
            table_object->command[0][i + 1] = table_object->command[0][i];
        }
        table_object->command[0][table_object->command_character_highlighted] = c;
        table_object->command_character_highlighted++;
        return 1;
    }
    return 0;
}

int on_edit_go_left(table_type table_object){
    if(table_object->active_line == -1){
        if(table_object->character_highlighted > 0){
            if(table_object->first_character_printed > 0 && table_object->character_highlighted == strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                table_object->first_character_printed--;
            }
            table_object->character_highlighted--;
            return 1;
        }
    }
    else{
        if(table_object->character_highlighted > 0){
            if(table_object->first_character_printed > 0 && table_object->character_highlighted == strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]])){
                table_object->first_character_printed--;
            }
            table_object->character_highlighted--;
            return 1;
        }
    }
    return 0;
}

int on_edit_go_right(table_type table_object){
    if(table_object->active_line == -1){
        if(table_object->character_highlighted < strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
            table_object->character_highlighted++;
            return 1;
        }
    }
    else{
        if(table_object->character_highlighted < strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]])){
            table_object->character_highlighted++;
            return 1;
        }
    }
    return 0;
}

int on_edit_backspace(table_type table_object){
    if(table_object->active_column > -1 && table_object->active_line == -1){
        if(table_object->character_highlighted > 0){
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
            return 1;
        }
    }
    else if(table_object->active_column > -1 && table_object->active_line > -1){
        if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > 0){
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
            return 1;
        }
    }
    return 0;
}

int on_edit_characters(table_type table_object, int cell_max_width, char c){
    if(table_object->active_column > -1 && table_object->active_line == -1){
        if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) < cell_max_width + 1){
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
            return 1;
        }
    }
    else if(table_object->active_column > -1 && table_object->active_line > -1){
        if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < cell_max_width + 1){
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
            return 1;
        }
    }
    return 0;
}