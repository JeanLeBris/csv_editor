#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef __linux__
#include <ncurses.h>
#endif
#ifdef _WIN64
#include <windows.h>
#endif
#include "../lib/table.h"

void Get_File_Characteristics(table_type table_object, config_type config){
    FILE *f = fopen(config->input_file, "r");
    if(f == NULL){
        // printf("file error\n");
        no_output_string_display("file error\n");
        return;
    }
    int col_num = 1;
    int line_num = 0;
    char *line = malloc((config->file_line_max_length + 1) * sizeof(*line));
    int i = 0;
    while(!feof(f) && (config->file_max_length == -1 || i < config->file_max_length)){
        line = fgets(line, config->file_line_max_length + 1, f);
        if(line != NULL){
            // printf(line);
            if(i == 0){
                for(int j = 0; j < strlen(line); j++){
                    if(line[j] == config->input_separator)
                        col_num++;
                }
                table_object->table_width = col_num;
            }
            else{

            }
            line_num++;
        }
        i++;
    }
    free(line);
    fclose(f);
    table_object->table_length = line_num - 1;
    return;
}

void Fetch_Data_From_Csv(table_type table_object, config_type config, int start_in_file, int end_in_file, int start_to_replace, int memory_to_replace){
    // char sep = ';'; // Follows the RFC 4180, to change later to accept other separators
    Get_File_Characteristics(table_object, config);
    FILE *f = fopen(config->input_file, "r");
    if(f == NULL){
        // printf("file error\n");
        no_output_string_display("file error\n");
        return;
    }
    int col_num = 0;
    int line_num = 0;
    int in_cell_iterator = 0;
    int in_quotes = 0;
    char *line = malloc((config->file_line_max_length + 1) * sizeof(*line));
    int i = 0;
    // printf("%d,%d\n", table_object->table_length, table_object->table_width);
    while(!feof(f) && (config->file_max_length == -1 || i < config->file_max_length)){
        line = fgets(line, config->file_line_max_length + 1, f);
        if(line != NULL){
            // printf(line);
            if(i == 0){ // if in header
                table_object->header = malloc(table_object->table_width * sizeof(*table_object->header));
                for(int j = 0; j < table_object->table_width; j++){
                    table_object->header[j] = malloc((config->cell_max_width + 1) * sizeof(char));
                    table_object->header[j][0] = '\0';
                }
                table_object->columns_order_of_display = malloc(table_object->table_width * sizeof(int));
                for(int j = 0; j < table_object->table_width; j++){
                    table_object->columns_order_of_display[j] = j;
                }
                table_object->cell_width = malloc(table_object->table_width * sizeof(int));
                for(int j = 0; j < table_object->table_width; j++){
                    table_object->cell_width[j] = 0;
                }
                col_num = 0;
                for(int j = 0; j < strlen(line); j++){
                    if(in_quotes != 1 && line[j] == config->input_separator){
                        table_object->header[col_num][in_cell_iterator] = '\0';
                        table_object->cell_width[col_num] = strlen(table_object->header[col_num]);
                        // printf("%s\n", table_object->header[col_num]);
                        col_num++;
                        in_cell_iterator = 0;
                        in_quotes = 0;
                    }
                    else if(in_quotes != 1 && ((line[j] == '\r' && line[j + 1] == '\n') || line[j] == '\n' || line[j] == '\0')){ // End of line either by CRLF or LF (\0 added to it in case there is no next line)
                        table_object->header[col_num][in_cell_iterator] = '\0';
                        table_object->cell_width[col_num] = strlen(table_object->header[col_num]);
                        // printf("%s\n", table_object->header[col_num]);
                        in_quotes = 0;
                    }
                    else if(in_quotes == 0 && line[j] == '\"'){
                        in_quotes++;
                    }
                    else if(in_quotes == 1 && line[j] == '\"' && line[j + 1] != '\"'){
                        in_quotes++;
                    }
                    else if(in_quotes == 1 && line[j] == '\"' && line[j + 1] == '\"'){
                        if(in_cell_iterator < config->cell_max_width){
                            table_object->header[col_num][in_cell_iterator] = line[j];
                            in_cell_iterator++;
                        }
                        j++;
                    }
                    else{
                        if(in_cell_iterator < config->cell_max_width){
                            if(line[j] == '\n'){
                                // table_object->header[col_num][in_cell_iterator] = '\n';
                                table_object->header[col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = strlen(table_object->header[col_num]);
                                in_quotes = 0;
                            }
                            else if(line[j] == '\r' && line[j + 1] == '\n'){
                                // table_object->header[col_num][in_cell_iterator] = '\n';
                                // j++;
                                table_object->header[col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = strlen(table_object->header[col_num]);
                                in_quotes = 0;
                            }
                            else if(line[j] == '\0'){
                                table_object->header[col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = strlen(table_object->header[col_num]);
                                in_quotes = 0;
                            }
                            else{
                                table_object->header[col_num][in_cell_iterator] = line[j];
                            }
                            in_cell_iterator++;
                        }
                        else{
                            table_object->header[col_num][config->cell_max_width - 1] = '\0';
                        }
                    }
                }
            }
            else{   // if in body
                if(i == 1){
                    // Unclean way of declaring the table, to change
                    table_object->table = malloc(table_object->table_length * sizeof(char **));
                    for(int j = 0; j < table_object->table_length; j++){
                        table_object->table[j] = malloc(table_object->table_width * sizeof(char *));
                        for(int k = 0; k < table_object->table_width; k++){
                            table_object->table[j][k] = malloc((config->cell_max_width + 1) * sizeof(char));
                            table_object->table[j][k][0] = '\0';
                        }
                    }

                    // // To make a cleaner declaration but doesn't work as intended
                    // table_object->table = malloc(sizeof(char[table_object->table_length][table_object->table_width][config->cell_max_width + 1]));

                    line_num = 0;
                }
                col_num = 0;
                in_cell_iterator = 0;
                for(int j = 0; j < strlen(line) + 1; j++){  // +1 to be sure of getting a '\0' character
                    if(in_quotes != 1 && line[j] == config->input_separator){
                        table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                        if(strlen(table_object->table[line_num][col_num]) > table_object->cell_width[col_num])
                            table_object->cell_width[col_num] = strlen(table_object->table[line_num][col_num]);
                        // printf("%s\n", table_object->table[line_num][col_num]);
                        col_num++;
                        in_cell_iterator = 0;
                        in_quotes = 0;
                    }
                    else if(in_quotes != 1 && ((line[j] == '\r' && line[j + 1] == '\n') || line[j] == '\n' || line[j] == '\0')){ // End of line either by CRLF or LF (\0 added to it in case there is no next line)
                        table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                        if(strlen(table_object->table[line_num][col_num]) > table_object->cell_width[col_num])
                            table_object->cell_width[col_num] = strlen(table_object->table[line_num][col_num]);
                        // printf("%s\n", table_object->table[line_num][col_num]);
                        in_quotes = 0;
                    }
                    else if(in_quotes == 0 && line[j] == '\"'){
                        in_quotes++;
                    }
                    else if(in_quotes == 1 && line[j] == '\"' && line[j + 1] != '\"'){
                        in_quotes++;
                    }
                    else if(in_quotes == 1 && line[j] == '\"' && line[j + 1] == '\"'){
                        if(in_cell_iterator < config->cell_max_width){
                            table_object->table[line_num][col_num][in_cell_iterator] = line[j];
                            in_cell_iterator++;
                        }
                        j++;
                    }
                    else{
                        if(in_cell_iterator < config->cell_max_width){
                            if(line[j] == '\n'){
                                // table_object->table[line_num][col_num][in_cell_iterator] = '\n';
                                table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = strlen(table_object->table[line_num][col_num]);
                                in_quotes = 0;
                            }
                            else if(line[j] == '\r' && line[j + 1] == '\n'){
                                // table_object->table[line_num][col_num][in_cell_iterator] = '\n';
                                // j++;
                                table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = strlen(table_object->table[line_num][col_num]);
                                in_quotes = 0;
                            }
                            else if(line[j] == '\0'){
                                table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = strlen(table_object->table[line_num][col_num]);
                                in_quotes = 0;
                            }
                            else{
                                table_object->table[line_num][col_num][in_cell_iterator] = line[j];
                            }
                            in_cell_iterator++;
                        }
                        else{
                            table_object->table[line_num][col_num][config->cell_max_width - 1] = '\0';
                        }
                    }
                }
                line_num++;
            }
        }
        i++;
    }
    // for(int i = 0; i < table_object->table_width; i++){
    //     printf("%s\n", table_object->header[i]);
    // }
    // for(int i = 0; i < table_object->table_length; i++){
    //     for(int j = 0; j < table_object->table_width; j++){
    //         printf("%s\n", table_object->table[i][j]);
    //     }
    // }
    // for(int i = 0; i < table_object->table_width; i++){
    //     printf("%d\n", table_object->cell_width[i]);
    // }
    // for(int i = 0; i < table_object->table_width; i++){
    //     printf("%d\n", table_object->columns_order_of_display[i]);
    // }
    // printf("%d\t%d", table_object->table_length, table_object->table_width);
    free(line);
    fclose(f);
}

void Print_Table(table_type table_object, config_type config){
    char *output = NULL;
    #ifdef __linux__
    Scrollback_To_Screen_Start();
    #endif
    #ifdef _WIN64
    output = (char*)malloc(config->window_length * config->window_width * 2 * sizeof(char));
    strcpy(output, "\0");
    #endif
    Hide_Cursor(output);
    int width_of_cell_changed = 0;
    int difference = 0;
    char char_buffer[10] = " ";
    int width_counter = 0;
    // Clear_Screen_By_Scrolldown();
    int amount_of_rows_reserved_for_content_other_than_rows = 5;    // the header is not considered a row
    int max_lines_to_print = config->window_length - amount_of_rows_reserved_for_content_other_than_rows;
    int sum = 0;
    int iteration_limit = 10;
    int iterator = 0;
    if(table_object->active_line < table_object->first_line_printed){
        if(table_object->active_line >= 0){
            table_object->first_line_printed = table_object->active_line;
        }
        else{
            table_object->first_line_printed = 0;
        }
    }
    else if(table_object->active_line > table_object->first_line_printed + max_lines_to_print - 1){
        table_object->first_line_printed = table_object->active_line - max_lines_to_print + 1;
    }
    if(table_object->character_highlighted != -1){
        if(table_object->active_line == -1){
            if(strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) + 1 > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]) + 1;
                width_of_cell_changed = 1;
            }
        }
        else{
            if(strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) + 1 > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]) + 1;
                width_of_cell_changed = 1;
            }
        }
    }
    sum = config->window_width + 1;
    iterator = 0;
    while(sum > config->window_width && iterator < iteration_limit){
        sum = 1;
        for(int i = table_object->first_column_printed; i <= table_object->active_column; i++){
            if(i == table_object->active_column){
                if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                    sum += table_object->cell_width[table_object->columns_order_of_display[i]] + 1;
                }
                else{
                    sum += config->focused_cell_max_width + 1;
                }
            }
            else{
                if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                    sum += table_object->cell_width[table_object->columns_order_of_display[i]] + 1;
                }
                else{
                    sum += config->unfocused_cell_max_width + 1;
                }
            }
        }
        if(sum > config->window_width){
            table_object->first_column_printed++;
        }
        else if(sum == 1 && table_object->first_column_printed > 0){
            table_object->first_column_printed--;
        }
        iterator++;
    }
    // sum = config->focused_cell_max_width + 1;
    if(table_object->character_highlighted > -1){
        if(table_object->active_column > -1 && table_object->active_line == -1){
            if(table_object->character_highlighted == strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                // table_object->first_character_printed = 1;
                if(table_object->character_highlighted + 1 > config->focused_cell_max_width){
                    table_object->first_character_printed = table_object->character_highlighted - config->focused_cell_max_width + 1;
                }
                else{
                    table_object->first_character_printed = 0;
                }
            }
            else if(table_object->character_highlighted < table_object->first_character_printed){
                table_object->first_character_printed = table_object->character_highlighted;
            }
            else if(table_object->character_highlighted > table_object->first_character_printed + config->focused_cell_max_width - 1){
                table_object->first_character_printed = table_object->character_highlighted - config->focused_cell_max_width + 1;
            }
        }
        else if(table_object->active_column > -1 && table_object->active_line > -1){
            if(table_object->character_highlighted == strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]])){
                // table_object->first_character_printed = 1;
                if(table_object->character_highlighted + 1 > config->focused_cell_max_width){
                    table_object->first_character_printed = table_object->character_highlighted - config->focused_cell_max_width + 1;
                }
                else{
                    table_object->first_character_printed = 0;
                }
            }
            else if(table_object->character_highlighted < table_object->first_character_printed){
                table_object->first_character_printed = table_object->character_highlighted;
            }
            else if(table_object->character_highlighted > table_object->first_character_printed + config->focused_cell_max_width - 1){
                table_object->first_character_printed = table_object->character_highlighted - config->focused_cell_max_width + 1;
            }
        }

        // if(table_object->active_column > -1 && table_object->active_line > -2){
        //     if(table_object->character_highlighted - table_object->first_character_printed + 1 > config->focused_cell_max_width){
        //         if(table_object->character_highlighted + 1 > config->focused_cell_max_width){
        //             table_object->first_character_printed = table_object->character_highlighted + 1 - config->focused_cell_max_width;
        //         }
        //         else{
        //             table_object->first_character_printed = 0;
        //         }
        //     }
        //     else if(table_object->character_highlighted - table_object->first_character_printed + 1 < 1){
        //         table_object->first_character_printed = table_object->character_highlighted;
        //     }
        // }
    }

    // Header part

    if(table_object->table_width > 0){
        Default_Colors(config, output);
        add_to_display_buffer("+", output);
        width_counter++;
    }
    for(int i = table_object->first_column_printed; i < table_object->table_width; i++){
        if(i == table_object->active_column){
            if(table_object->active_line == -2){
                Selection_Content_Colors(config, output);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->focused_cell_max_width && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
            Default_Colors(config, output);
        }
        else{
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->unfocused_cell_max_width && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
        }
        if(width_counter < config->window_width){
            add_to_display_buffer("+", output);
            width_counter++;
        }
    }
    while(width_counter < config->window_width){
        add_to_display_buffer(" ", output);
        width_counter++;
    }
    width_counter = 0;
    if(table_object->table_width > 0){
        if(table_object->active_line == -1 && table_object->active_column == -1){                // To fix
            Selection_Content_Colors(config, output);
        }
        else{
            Default_Colors(config, output);
        }
        add_to_display_buffer("|", output);
        width_counter++;
    }
    for(int i = table_object->first_column_printed; i < table_object->table_width; i++){
        if(i == table_object->active_column){   // if header is in active column
            if(table_object->character_highlighted == -1 && (table_object->active_line == -1 || table_object->active_line == -2)){
                Selection_Content_Colors(config, output);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){   // if cell width of header fits in displayed cell
                if(table_object->active_line != -1 || (table_object->character_highlighted < (int)strlen(table_object->header[table_object->columns_order_of_display[i]]))){ // strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])
                    difference = table_object->cell_width[table_object->columns_order_of_display[i]] - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                }
                else{
                    difference = table_object->cell_width[table_object->columns_order_of_display[i]] - strlen(table_object->header[table_object->columns_order_of_display[i]]) - 1;
                }
                // sprintf(char_buffer, "%d", (int)strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]));
                // strcat(output, char_buffer);
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    add_to_display_buffer(" ", output);
                    width_counter++;
                }
                // strcat(output, table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < strlen(table_object->header[table_object->columns_order_of_display[i]]) && width_counter < config->window_width; j++){
                    char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                    if(table_object->active_line == -1 && j == table_object->character_highlighted){
                        Selection_Content_Colors(config, output);
                    }
                    add_to_display_buffer(char_buffer, output);
                    if(table_object->active_line == -1 && j == table_object->character_highlighted){
                        Default_Colors(config, output);
                    }
                    width_counter++;
                }
                if(table_object->active_line == -1 && table_object->character_highlighted == strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                    Selection_Content_Colors(config, output);
                    add_to_display_buffer(" ", output);
                    Default_Colors(config, output);
                    width_counter++;
                }
            }
            else{   // if cell width of header doesn't fit in displayed cell
                if(table_object->active_line != -1 || (table_object->character_highlighted < (int)strlen(table_object->header[table_object->columns_order_of_display[i]]))){
                    difference = config->focused_cell_max_width - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                }
                else{
                    difference = config->focused_cell_max_width - strlen(table_object->header[table_object->columns_order_of_display[i]]) - 1;
                }
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    add_to_display_buffer(" ", output);
                    width_counter++;
                }
                if(difference < 0){ // if header width doesn't fit in displayed cell
                    // if(table_object->active_line == -1 && table_object->character_highlighted == strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                    if(table_object->active_line == -1 && table_object->character_highlighted != -1){
                        for(int j = table_object->first_character_printed; j < table_object->first_character_printed + config->focused_cell_max_width && width_counter < config->window_width; j++){
                            char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                            if(table_object->active_line == -1 && j == table_object->character_highlighted){
                                Selection_Content_Colors(config, output);
                            }
                            add_to_display_buffer(char_buffer, output);
                            if(table_object->active_line == -1 && j == table_object->character_highlighted){
                                Default_Colors(config, output);
                            }
                            // strcat(output, table_object->header[table_object->columns_order_of_display[i]][j]);
                            width_counter++;
                        }
                    }
                    else{
                        for(int j = 0; j < config->focused_cell_max_width && width_counter < config->window_width; j++){
                            char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                            // if(j == table_object->character_highlighted){
                            //     S_Selection_Content_Colors(config, output);
                            // }
                            add_to_display_buffer(char_buffer, output);
                            // if(j == table_object->character_highlighted){
                            //     S_Default_Colors(config, output);
                            // }
                            // strcat(output, table_object->header[table_object->columns_order_of_display[i]][j]);
                            width_counter++;
                        }
                    }
                }
                else{   // if header width fits in displayed cell
                    // strcat(output, table_object->header[table_object->columns_order_of_display[i]]);
                    for(int j = 0; j < strlen(table_object->header[table_object->columns_order_of_display[i]]) && width_counter < config->window_width; j++){
                        char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                        if(table_object->active_line == -1 && j == table_object->character_highlighted){
                            Selection_Content_Colors(config, output);
                        }
                        add_to_display_buffer(char_buffer, output);
                        if(table_object->active_line == -1 && j == table_object->character_highlighted){
                            Default_Colors(config, output);
                        }
                        width_counter++;
                    }
                }
                if(table_object->active_line == -1 && table_object->character_highlighted == strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                    Selection_Content_Colors(config, output);
                    add_to_display_buffer(" ", output);
                    Default_Colors(config, output);
                    width_counter++;
                }
            }
            Default_Colors(config, output);
        }
        else{   // if header not in active column
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                difference = table_object->cell_width[table_object->columns_order_of_display[i]] - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    add_to_display_buffer(" ", output);
                    width_counter++;
                }
                // strcat(output, table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < strlen(table_object->header[table_object->columns_order_of_display[i]]) && width_counter < config->window_width; j++){
                    char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                    add_to_display_buffer(char_buffer, output);
                    width_counter++;
                }
            }
            else{
                difference = config->unfocused_cell_max_width - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    add_to_display_buffer(" ", output);
                    width_counter++;
                }
                if(difference < 0){
                    for(int j = 0; j < config->unfocused_cell_max_width && width_counter < config->window_width; j++){
                        char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                        add_to_display_buffer(char_buffer, output);
                        // strcat(output, table_object->header[table_object->columns_order_of_display[i]][j]);
                        width_counter++;
                    }
                }
                else{
                    // strcat(output, table_object->header[table_object->columns_order_of_display[i]]);
                    for(int j = 0; j < strlen(table_object->header[table_object->columns_order_of_display[i]]) && width_counter < config->window_width; j++){
                        char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                        add_to_display_buffer(char_buffer, output);
                        width_counter++;
                    }
                }
            }
        }
        if(width_counter < config->window_width){
            add_to_display_buffer("|", output);
            width_counter++;
        }
    }
    if(table_object->active_line == -1 && table_object->active_column == -1){
        Default_Colors(config, output);
    }
    while(width_counter < config->window_width){
        add_to_display_buffer(" ", output);
        width_counter++;
    }
    width_counter = 0;
    if(table_object->table_width > 0){
        Default_Colors(config, output);
        add_to_display_buffer("+", output);
        width_counter++;
    }
    for(int i = table_object->first_column_printed; i < table_object->table_width; i++){
        if(i == table_object->active_column){
            if(table_object->active_line == -2){
                Selection_Content_Colors(config, output);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->focused_cell_max_width && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
            Default_Colors(config, output);
        }
        else{
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->unfocused_cell_max_width && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
        }
        if(width_counter < config->window_width){
            add_to_display_buffer("+", output);
            width_counter++;
        }
    }
    while(width_counter < config->window_width){
        add_to_display_buffer(" ", output);
        width_counter++;
    }

    // Body part

    for(int i = table_object->first_line_printed; i < table_object->table_length && (i - table_object->first_line_printed) < max_lines_to_print; i++){
        width_counter = 0;
        if((i + 1)%2 == 0){
            Even_Line_Colors(config, output);
        }
        else{
            Odd_Line_Colors(config, output);
        }
        if(i == table_object->active_line && table_object->active_column == -1){
            Selection_Content_Colors(config, output);
        }
        add_to_display_buffer("|", output);
        width_counter++;
        for(int j = table_object->first_column_printed; j < table_object->table_width; j++){
            if(j == table_object->active_column){   // if cell in active column
                if(table_object->character_highlighted == -1 && (table_object->active_line == i || table_object->active_line == -2)){
                    Selection_Content_Colors(config, output);
                }
                if(table_object->cell_width[table_object->columns_order_of_display[j]] < config->focused_cell_max_width){   // if cell width of cell fits in displayed cell
                    if(i != table_object->active_line || (table_object->character_highlighted < (int)strlen(table_object->table[i][table_object->columns_order_of_display[j]]))){
                        difference = table_object->cell_width[table_object->columns_order_of_display[j]] - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    }
                    else{
                        difference = table_object->cell_width[table_object->columns_order_of_display[j]] - strlen(table_object->table[i][table_object->columns_order_of_display[j]]) - 1;
                    }
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        add_to_display_buffer(" ", output);
                        width_counter++;
                    }
                    // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                        char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                        if(table_object->active_line == i && k == table_object->character_highlighted){
                            Selection_Content_Colors(config, output);
                        }
                        add_to_display_buffer(char_buffer, output);
                        if(table_object->active_line == i && k == table_object->character_highlighted){
                            if((i + 1)%2 == 0){
                                Even_Line_Colors(config, output);
                            }
                            else{
                                Odd_Line_Colors(config, output);
                            }
                        }
                        width_counter++;
                    }
                    if(table_object->active_line == i && table_object->character_highlighted == strlen(table_object->table[i][table_object->columns_order_of_display[table_object->active_column]])){
                        Selection_Content_Colors(config, output);
                        add_to_display_buffer(" ", output);
                        Default_Colors(config, output);
                        width_counter++;
                    }
                }
                else{   // if cell width of cell doesn't fit in displayed cell
                    if(i != table_object->active_line || (table_object->character_highlighted < (int)strlen(table_object->table[i][table_object->columns_order_of_display[j]]))){
                        difference = config->focused_cell_max_width - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    }
                    else{
                        difference = config->focused_cell_max_width - strlen(table_object->table[i][table_object->columns_order_of_display[j]]) - 1;
                    }
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        add_to_display_buffer(" ", output);
                        width_counter++;
                    }
                    if(difference < 0){ // if cell width doesn't fit in displayed cell
                        if(i == table_object->active_line && table_object->character_highlighted != -1){
                            for(int k = table_object->first_character_printed; k < table_object->first_character_printed + config->focused_cell_max_width && width_counter < config->window_width; k++){
                                char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                                if(table_object->active_line == i && k == table_object->character_highlighted){
                                    Selection_Content_Colors(config, output);
                                }
                                add_to_display_buffer(char_buffer, output);
                                if(table_object->active_line == i && k == table_object->character_highlighted){
                                    if((i + 1)%2 == 0){
                                        Even_Line_Colors(config, output);
                                    }
                                    else{
                                        Odd_Line_Colors(config, output);
                                    }
                                }
                                // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]][k]);
                                width_counter++;
                            }
                        }
                        else{
                            for(int k = 0; k < config->focused_cell_max_width && width_counter < config->window_width; k++){
                                char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                                add_to_display_buffer(char_buffer, output);
                                // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]][k]);
                                width_counter++;
                            }
                        }
                    }
                    else{   // if cell width fits in displayed cell
                        // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                        for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                            char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                            if(table_object->active_line == i && k == table_object->character_highlighted){
                                Selection_Content_Colors(config, output);
                            }
                            add_to_display_buffer(char_buffer, output);
                            if(table_object->active_line == i && k == table_object->character_highlighted){
                                if((i + 1)%2 == 0){
                                    Even_Line_Colors(config, output);
                                }
                                else{
                                    Odd_Line_Colors(config, output);
                                }
                            }
                            width_counter++;
                        }
                    }
                    if(table_object->active_line == i && table_object->character_highlighted == strlen(table_object->table[i][table_object->columns_order_of_display[table_object->active_column]])){
                        Selection_Content_Colors(config, output);
                        add_to_display_buffer(" ", output);
                        Default_Colors(config, output);
                        width_counter++;
                    }
                }
                // Default_Colors(config);
                if((i + 1)%2 == 0){
                    Even_Line_Colors(config, output);
                }
                else{
                    Odd_Line_Colors(config, output);
                }
            }
            else{   // if cell not in active column
                if(table_object->cell_width[table_object->columns_order_of_display[j]] < config->unfocused_cell_max_width){
                    difference = table_object->cell_width[table_object->columns_order_of_display[j]] - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        add_to_display_buffer(" ", output);
                        width_counter++;
                    }
                    // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                        char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                        add_to_display_buffer(char_buffer, output);
                        width_counter++;
                    }
                }
                else{
                    difference = config->unfocused_cell_max_width - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        add_to_display_buffer(" ", output);
                        width_counter++;
                    }
                    if(difference < 0){
                        for(int k = 0; k < config->unfocused_cell_max_width && width_counter < config->window_width; k++){
                            char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                            add_to_display_buffer(char_buffer, output);
                            // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]][k]);
                            width_counter++;
                        }
                    }
                    else{
                        // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                        for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                            char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                            add_to_display_buffer(char_buffer, output);
                            width_counter++;
                        }
                    }
                }
            }
            if(width_counter < config->window_width){
                add_to_display_buffer("|", output);
                width_counter++;
            }
        }
        // if(i == table_object->active_line && table_object->active_column == -1){
        //     S_Default_Colors(config, output);
        // }
        Default_Colors(config, output);
        while(width_counter < config->window_width){
            add_to_display_buffer(" ", output);
            width_counter++;
        }
    }

    // Last line

    width_counter = 0;
    if(table_object->table_width > 0){
        Default_Colors(config, output);
        add_to_display_buffer("+", output);
        width_counter++;
    }
    for(int i = table_object->first_column_printed; i < table_object->table_width; i++){
        if(i == table_object->active_column){
            if(table_object->active_line == -2){
                Selection_Content_Colors(config, output);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->focused_cell_max_width && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
            Default_Colors(config, output);
        }
        else{
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->unfocused_cell_max_width && width_counter < config->window_width; j++){
                    add_to_display_buffer("-", output);
                    width_counter++;
                }
            }
        }
        if(width_counter < config->window_width){
            add_to_display_buffer("+", output);
            width_counter++;
        }
    }
    while(width_counter < config->window_width){
        add_to_display_buffer(" ", output);
        width_counter++;
    }
    for(int i = table_object->table_length; i < max_lines_to_print; i++){
        for(int j = 0; j < config->window_width; j++){
            add_to_display_buffer(" ", output);
        }
    }
    // sprintf(char_buffer, "%d\t%d\t", table_object->first_character_printed, table_object->character_highlighted);
    // strcat(output, char_buffer);
    // strcat(output, table_object->command);
    for(int i = 0; i < strlen(table_object->command[table_object->active_command]); i++){
        if(i == table_object->command_character_highlighted){
            Selection_Content_Colors(config, output);
        }
        char_buffer[0] = table_object->command[table_object->active_command][i];
        add_to_display_buffer(char_buffer, output);
        if(i == table_object->command_character_highlighted){
            Default_Colors(config, output);
        }
    }
    //add_to_display_buffer(table_object->command[0]);
    if(table_object->command_character_highlighted == strlen(table_object->command[table_object->active_command])){
        Selection_Content_Colors(config, output);
        add_to_display_buffer(" ", output);
        Default_Colors(config, output);
        for(int i = strlen(table_object->command[table_object->active_command]) + 1; i < config->window_width; i++){
            add_to_display_buffer(" ", output);
        }
    }
    else{
        for(int i = strlen(table_object->command[table_object->active_command]); i < config->window_width; i++){
            add_to_display_buffer(" ", output);
        }
    }
    
    if(table_object->character_highlighted != -1 && width_of_cell_changed){
        if(table_object->active_line == -1){
            table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]);
        }
        else{
            table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]]);
        }
    }

    #ifdef _WIN64
    Scrollback_To_Screen_Start();
    #endif
    display(output);
    #ifdef _WIN64
    free(output);
    #endif
}

void Update_Cell_Width_By_Column(table_type table_object, int displayed_column_id){
    table_object->cell_width[table_object->columns_order_of_display[displayed_column_id]] = strlen(table_object->header[table_object->columns_order_of_display[displayed_column_id]]);
    for(int i = 0; i < table_object->table_length; i++){
        if(strlen(table_object->table[i][table_object->columns_order_of_display[displayed_column_id]]) > table_object->cell_width[table_object->columns_order_of_display[displayed_column_id]]){
            table_object->cell_width[table_object->columns_order_of_display[displayed_column_id]] = strlen(table_object->table[i][table_object->columns_order_of_display[displayed_column_id]]);
        }
    }
}

table_type Free_Table_Object(table_type table_object, config_type config){
    for(int j = 0; j < table_object->table_length; j++){
        for(int k = 0; k < table_object->table_width; k++){
            free(table_object->table[j][k]);
        }
        free(table_object->table[j]);
    }
    free(table_object->table);

    for(int j = 0; j < table_object->table_width; j++){
        free(table_object->header[j]);
    }
    free(table_object->header);
    
    free(table_object->columns_order_of_display);
    
    free(table_object->cell_width);

    for(int j = 0; j < config->commands_history_length + 1; j++){
        free(table_object->command[j]);
    }
    free(table_object->command);

    free(table_object);

    return NULL;
}