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

int execute_command(table_type table_object, config_type config, int commands_history_length){
    int buffer = 0;
    int running = 1;
    char *command_buffer = NULL;
    char seps[] = " \n\"";
    if(table_object->active_command != 0){
        strcpy(table_object->command[0], table_object->command[table_object->active_command]);
    }
    command_buffer = table_object->command[0];
    if(strcmp(command_buffer, ":q") == 0){
        running = 0;
    }
    else if(strstr(command_buffer, ":wq") == command_buffer){
        command_buffer = strstr(command_buffer, ":wq") + strlen(":wq");
        command_buffer = strtok(command_buffer, seps);
        if(command_buffer != NULL){
            strcpy(config->input_file, command_buffer);
        }
        Save_Table(table_object, config);
        
        running = 0;
    }
    else if(strstr(command_buffer, ":jump ") == command_buffer){
        command_buffer = strstr(command_buffer, ":jump ") + strlen(":jump ");
        buffer = atoi(command_buffer);
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
    else if(strstr(command_buffer, ":new ") == command_buffer){
        command_buffer = strstr(command_buffer, ":new ") + strlen(":new ");
        if(strstr(command_buffer, "column") == command_buffer){
            command_buffer = strstr(command_buffer, "column") + strlen("column");

            char **new_header = malloc((table_object->table_width + 1) * sizeof(*table_object->header));
            for(int j = 0; j < table_object->active_column + 1; j++){
                new_header[j] = table_object->header[table_object->columns_order_of_display[j]];
            }
            new_header[table_object->active_column + 1] = malloc((config->cell_max_width + 1) * sizeof(char));
            new_header[table_object->active_column + 1][0] = '\0';
            for(int j = table_object->active_column + 1; j < table_object->table_width; j++){
                new_header[j+1] = table_object->header[table_object->columns_order_of_display[j]];
            }
            free(table_object->header);
            table_object->header = new_header;

            int *new_cell_width = malloc((table_object->table_width + 1) * sizeof(int));
            for(int j = 0; j < table_object->active_column + 1; j++){
                new_cell_width[j] = table_object->cell_width[table_object->columns_order_of_display[j]];
            }
            new_cell_width[table_object->active_column + 1] = 5;
            for(int j = table_object->active_column + 1; j < table_object->table_width; j++){
                new_cell_width[j+1] = table_object->cell_width[table_object->columns_order_of_display[j]];
            }
            free(table_object->cell_width);
            table_object->cell_width = new_cell_width;

            for(int j = 0; j < table_object->table_length; j++){
                char **new_table_line = malloc((table_object->table_width + 1) * sizeof(char *));
                // table_object->table[j] = malloc(table_object->table_width * sizeof(char *));
                for(int k = 0; k < table_object->active_column + 1; k++){
                    new_table_line[k] = table_object->table[j][table_object->columns_order_of_display[k]];
                }
                new_table_line[table_object->active_column + 1] = malloc((config->cell_max_width + 1) * sizeof(char));
                new_table_line[table_object->active_column + 1][0] = '\0';
                for(int k = table_object->active_column + 1; k < table_object->table_width; k++){
                    new_table_line[k+1] = table_object->table[j][table_object->columns_order_of_display[k]];
                }
                free(table_object->table[j]);
                table_object->table[j] = new_table_line;
            }

            int *new_columns_order_of_display = malloc((table_object->table_width + 1) * sizeof(int));
            for(int j = 0; j < table_object->table_width + 1; j++){
                new_columns_order_of_display[j] = j;
            }
            free(table_object->columns_order_of_display);
            table_object->columns_order_of_display = new_columns_order_of_display;

            table_object->table_width++;
            table_object->active_column++;
        }
        if(strstr(command_buffer, "line") == command_buffer){
            command_buffer = strstr(command_buffer, "line") + strlen("line");

            char ***new_table = malloc((table_object->table_length + 1) * sizeof(char **));
            for(int j = 0; j < table_object->active_line + 1; j++){
                new_table[j] = table_object->table[j];
            }
            new_table[table_object->active_line + 1] = malloc(table_object->table_width * sizeof(char *));
            for(int k = 0; k < table_object->table_width; k++){
                new_table[table_object->active_line + 1][k] = malloc((config->cell_max_width + 1) * sizeof(char));
                new_table[table_object->active_line + 1][k][0] = '\0';
            }
            for(int j = table_object->active_line + 1; j < table_object->table_length; j++){
                new_table[j+1] = table_object->table[j];
            }
            free(table_object->table);
            table_object->table = new_table;

            table_object->table_length++;
            table_object->active_line++;
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