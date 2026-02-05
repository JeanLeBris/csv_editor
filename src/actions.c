#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/actions.h"
#include "../lib/constants.h"
#include "../lib/utils.h"

void set_character_highlighted_to_last_character(table_type table_object){
    if(table_object->active_line == -1){
        table_object->character_highlighted = strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]);
    }
    else{
        table_object->character_highlighted = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
    }
    table_object->first_character_printed = 0;
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

int execute_command(table_type table_object, config_type config, int commands_history_length, display_text_type display_text){
    int buffer = 0;
    int running = 1;
    char *command_buffer = NULL;
    char seps[] = " \n\"";
    char *buffer_string = malloc(1 * sizeof(char));
    buffer_string[0] = '\0';
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
            int *new_type = malloc((table_object->table_width + 1) * sizeof(*table_object->type));
            for(int j = 0; j < table_object->active_column + 1; j++){
                new_header[j] = table_object->header[table_object->columns_order_of_display[j]];
                new_type[j] = table_object->type[table_object->columns_order_of_display[j]];
            }
            new_header[table_object->active_column + 1] = malloc((config->cell_max_width + 1) * sizeof(char));
            new_header[table_object->active_column + 1][0] = '\0';
            new_type[table_object->active_column + 1] = COLUMN_TYPE_INTEGER;
            for(int j = table_object->active_column + 1; j < table_object->table_width; j++){
                new_header[j+1] = table_object->header[table_object->columns_order_of_display[j]];
                new_type[j+1] = table_object->type[table_object->columns_order_of_display[j]];
            }
            free(table_object->header);
            free(table_object->type);
            table_object->header = new_header;
            table_object->type = new_type;

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
    else if(strstr(command_buffer, ":exec(")){
        buffer = 1;
        for(char *command_buffer_substring = &command_buffer[6]; command_buffer_substring[0] != '\0'; command_buffer_substring = &command_buffer_substring[1]){
            if(command_buffer_substring[0] == '('){
                buffer++;
            }
            else if(command_buffer_substring[0] == ')'){
                buffer--;
            }
            if(buffer == 0){
                break;
            }
        }
        if(buffer == 0){    // If exec command has the right count of '(' and ')'
            for(char *command_buffer_substring = &command_buffer[5]; command_buffer_substring[0] != '\0'; command_buffer_substring = &command_buffer_substring[1]){
                if(strstr(command_buffer_substring, "get ") == command_buffer_substring){
                    command_buffer_substring = strstr(command_buffer_substring, "get ") + strlen("get ");
                    if(strstr(command_buffer_substring, "column_types ") == command_buffer_substring){
                        command_buffer_substring = strstr(command_buffer_substring, "column_types ") + strlen("column_types");
                        for(int i = 0; i < table_object->table_width; i++){
                            buffer_string = realloc(buffer_string, strlen(buffer_string) + 3);
                            sprintf(buffer_string, "%s%d\n", buffer_string, table_object->type[table_object->columns_order_of_display[i]]);
                        }
                    }
                }
                else if(strstr(command_buffer_substring, "> ") == command_buffer_substring){
                    command_buffer_substring = strstr(command_buffer_substring, "> ") + strlen("> ");
                    FILE* output_file = fopen(strtok(command_buffer_substring, " )"), "w");
                    fprintf(output_file, buffer_string);
                    fclose(output_file);
                    buffer_string = realloc(buffer_string, 1 * sizeof(char));
                    buffer_string[0] = '\0';
                }
            }
            Update_Display_Text(display_text, buffer_string, BOTTOM_TEXT + REGULAR_TEXT);
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
    free(buffer_string);
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

int on_edit_go_left(table_type table_object, config_type config){
    if(table_object->active_line == -1){
        if(table_object->character_highlighted > 0){
            if(table_object->first_character_printed > 0 && table_object->character_highlighted == strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                table_object->first_character_printed -= length_of_last_character(&table_object->header[table_object->columns_order_of_display[table_object->active_column]][table_object->first_character_printed], config);
            }
            table_object->character_highlighted -= length_of_last_character(&table_object->header[table_object->columns_order_of_display[table_object->active_column]][table_object->character_highlighted], config);
            return 1;
        }
    }
    else{
        if(table_object->character_highlighted > 0){
            if(table_object->first_character_printed > 0 && table_object->character_highlighted == strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]])){
                table_object->first_character_printed -= length_of_last_character(&table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][table_object->first_character_printed], config);
            }
            table_object->character_highlighted -= length_of_last_character(&table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][table_object->character_highlighted], config);
            return 1;
        }
    }
    return 0;
}

int on_edit_go_right(table_type table_object, config_type config){
    if(table_object->active_line == -1){
        if(table_object->character_highlighted < strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
            table_object->character_highlighted += length_of_first_character(&table_object->header[table_object->columns_order_of_display[table_object->active_column]][table_object->character_highlighted], config);
            return 1;
        }
    }
    else{
        if(table_object->character_highlighted < strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]])){
            table_object->character_highlighted += length_of_first_character(&table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][table_object->character_highlighted], config);
            return 1;
        }
    }
    return 0;
}

int on_edit_backspace(table_type table_object, int cell_max_width){
    int is_input_ok = 0;
    char *buffer_string = malloc((cell_max_width + 1) * sizeof(char));
    // If in header
    if(table_object->active_column > -1 && table_object->active_line == -1){
        // If you can delete content
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
            is_input_ok = 1;
        }
    }
    // If in body
    else if(table_object->active_column > -1 && table_object->active_line > -1){
        // If you can delete content
        if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > 0){
            strcpy(buffer_string, table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
            for(int i = table_object->character_highlighted - 1; i < strlen(buffer_string); i++){
                buffer_string[i] = buffer_string[i + 1];
            }
            switch(table_object->type[table_object->columns_order_of_display[table_object->active_column]]){
                case COLUMN_TYPE_STRING:
                    is_input_ok = 1;
                    break;
                case COLUMN_TYPE_DATE:
                    if(Is_Date(buffer_string, strlen(buffer_string))){
                        is_input_ok = 1;
                    }
                    break;
                case COLUMN_TYPE_DECIMAL:
                    if(Is_Decimal(buffer_string, strlen(buffer_string))){
                        is_input_ok = 1;
                    }
                    break;
                case COLUMN_TYPE_INTEGER:
                    if(Is_Integer(buffer_string, strlen(buffer_string))){
                        is_input_ok = 1;
                    }
                    break;
                default:
                    break;
            }
            if(is_input_ok){
                strcpy(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]], buffer_string);
                if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                    table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                }
                else if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                    Update_Cell_Width_By_Column(table_object, table_object->active_column);
                }
            }
        }
    }
    
    free(buffer_string);
    if(is_input_ok){
        table_object->character_highlighted--;
        return 1;
    }
    else
        return 0;
}

int on_edit_characters(table_type table_object, int cell_max_width, char c){
    int is_input_ok = 0;
    char *buffer_string = malloc((cell_max_width + 1) * sizeof(char));
    // If in header
    if(table_object->active_column > -1 && table_object->active_line == -1){
        // If there is still space left in the string (to verify)
        if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) < cell_max_width){
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
            is_input_ok = 1;
        }
    }
    // If in body
    else if(table_object->active_column > -1 && table_object->active_line > -1){
        // If there is still space left in the string (to verify)
        if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < cell_max_width){
            strcpy(buffer_string, table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
            if(table_object->type[table_object->columns_order_of_display[table_object->active_column]] == COLUMN_TYPE_DATE){
                buffer_string[table_object->character_highlighted] = c;
            }
            else{
                for(int i = strlen(buffer_string); i >= table_object->character_highlighted; i--){
                    buffer_string[i + 1] = buffer_string[i];
                }
                buffer_string[table_object->character_highlighted] = c;
            }
            switch(table_object->type[table_object->columns_order_of_display[table_object->active_column]]){
                case COLUMN_TYPE_STRING:
                    is_input_ok = 1;
                    break;
                case COLUMN_TYPE_DATE:
                    if(Is_Date(buffer_string, strlen(buffer_string))){
                        is_input_ok = 1;
                    }
                    break;
                case COLUMN_TYPE_DECIMAL:
                    if(Is_Decimal(buffer_string, strlen(buffer_string))){
                        is_input_ok = 1;
                    }
                    break;
                case COLUMN_TYPE_INTEGER:
                    if(Is_Integer(buffer_string, strlen(buffer_string))){
                        is_input_ok = 1;
                    }
                    break;
                default:
                    break;
            }
            if(is_input_ok){
                strcpy(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]], buffer_string);
                if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                    table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
                }
                else if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) < table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                    Update_Cell_Width_By_Column(table_object, table_object->active_column);
                }
            }
        }
    }
    
    free(buffer_string);
    if(is_input_ok){
        table_object->character_highlighted++;
        return 1;
    }
    else
        return 0;
}