#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#ifdef __linux__
#include <ncurses.h>
#endif
#ifdef _WIN64
#include <windows.h>
#endif
#include "../lib/table.h"
#include "../lib/constants.h"
#include "../lib/utils.h"

/**
 * Update the text written at the bottom of the UI
 * 
 * TODO : Update the Print_Table() function to be able to print the text in a dedicated window in the middle of the screen
 */
void Update_Display_Text(display_text_type display_text, char *text, int type){
    if(display_text->text != NULL){
        free(display_text->text);
    }
    display_text->text = malloc((strlen(text) + 1) + sizeof(char));
    strcpy(display_text->text, text);
    display_text->type = type;
}

/**
 * Get the characteristics of the input CSV file
 * 
 * Check if the file exists
 * Count the number of rows until the end or until the file_max_length is reached
 * Count the number of columns
 */
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

/**
 * Check the value of a cell to the type of value that its cell should contain
 * 
 * If the value of the cell doesn't fit the type of value of its cell, the type of the value is updated to fit the value inputed
 * 
 * (that description is not very good)
 */
void Update_Column_Type_By_Cell_Value(int *column_type, char* cell_value, int size){
    switch(*column_type){
        case COLUMN_TYPE_STRING:
            break;
        case COLUMN_TYPE_DATE:
            if(!Is_Date(cell_value, size)){
                *column_type = COLUMN_TYPE_STRING;
            }
            break;
        case COLUMN_TYPE_DECIMAL:
            if(!Is_Decimal(cell_value, size)){
                if(!Is_Date(cell_value, size)){
                    *column_type = COLUMN_TYPE_STRING;
                }
                else
                    *column_type = COLUMN_TYPE_DATE;
            }
            break;
        case COLUMN_TYPE_INTEGER:
            if(!Is_Integer(cell_value, size)){
                if(!Is_Decimal(cell_value, size)){
                    if(!Is_Date(cell_value, size)){
                        *column_type = COLUMN_TYPE_STRING;
                    }
                    else
                        *column_type = COLUMN_TYPE_DATE;
                }
                else
                    *column_type = COLUMN_TYPE_DECIMAL;
            }
            break;
        default:
            break;
    }
    // printf("%d\n", *column_type);
    return;
}

/**
 * Fetch the content of a CSV file
 * 
 * 
 */
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
    // Read until the end of the file or until file_max_length is reached
    while(!feof(f) && (config->file_max_length == -1 || i < config->file_max_length)){
        line = fgets(line, config->file_line_max_length + 1, f);
        // If the line is real
        if(line != NULL){
            // If in header
            if(i == 0){
                // If UTF-8 signature found, remove it
                if(line[0] == (char) 0xEF && line[1] == (char) 0xBB && line[2] == (char) 0xBF){
                    for(int i = 0; i < strlen(line) - 2; i++){
                        line[i] = line[i+3];
                    }
                }
                table_object->header = malloc(table_object->table_width * sizeof(*table_object->header));
                table_object->type = malloc(table_object->table_width * sizeof(int));
                for(int j = 0; j < table_object->table_width; j++){
                    table_object->header[j] = malloc((config->cell_max_width + 1) * sizeof(char));
                    table_object->header[j][0] = '\0';
                    table_object->type[j] = COLUMN_TYPE_INTEGER;
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
                    // End of cell
                    if(in_quotes != 1 && line[j] == config->input_separator){
                        table_object->header[col_num][in_cell_iterator] = '\0';
                        table_object->cell_width[col_num] = encoding_strlen(table_object->header[col_num], config);
                        col_num++;
                        in_cell_iterator = 0;
                        in_quotes = 0;
                    }
                    // End of line either by CRLF or LF (\0 added to it in case there is no next line)
                    else if(in_quotes != 1 && ((line[j] == '\r' && line[j + 1] == '\n') || line[j] == '\n' || line[j] == '\0')){
                        table_object->header[col_num][in_cell_iterator] = '\0';
                        table_object->cell_width[col_num] = encoding_strlen(table_object->header[col_num], config);
                        in_quotes = 0;
                    }
                    // Start of a quote
                    else if(in_quotes == 0 && line[j] == '\"'){
                        in_quotes++;
                    }
                    // End of a quote
                    else if(in_quotes == 1 && line[j] == '\"' && line[j + 1] != '\"'){
                        in_quotes++;
                    }
                    // Inline quote (quote to keep in the display)
                    else if(in_quotes == 1 && line[j] == '\"' && line[j + 1] == '\"'){
                        if(in_cell_iterator < config->cell_max_width){
                            table_object->header[col_num][in_cell_iterator] = line[j];
                            in_cell_iterator++;
                        }
                        j++;
                    }
                    // Other characters
                    else{
                        if(in_cell_iterator < config->cell_max_width){
                            if(line[j] == '\n'){
                                table_object->header[col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = encoding_strlen(table_object->header[col_num], config);
                                in_quotes = 0;
                            }
                            else if(line[j] == '\r' && line[j + 1] == '\n'){
                                table_object->header[col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = encoding_strlen(table_object->header[col_num], config);
                                in_quotes = 0;
                            }
                            else if(line[j] == '\0'){
                                table_object->header[col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = encoding_strlen(table_object->header[col_num], config);
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
            // If in body
            else{
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
                for(int j = 0; j < strlen(line); j++){  // +1 to be sure of getting a '\0' character (removed for now)
                    // End of cell
                    if(in_quotes != 1 && line[j] == config->input_separator){
                        table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                        if(encoding_strlen(table_object->table[line_num][col_num], config) > table_object->cell_width[col_num])
                            table_object->cell_width[col_num] = encoding_strlen(table_object->table[line_num][col_num], config);
                        Update_Column_Type_By_Cell_Value(&table_object->type[col_num], table_object->table[line_num][col_num], in_cell_iterator);
                        col_num++;
                        in_cell_iterator = 0;
                        in_quotes = 0;
                    }
                    // End of line either by CRLF or LF (\0 added to it in case there is no next line)
                    else if(in_quotes != 1 && ((line[j] == '\r' && line[j + 1] == '\n') || line[j] == '\n' || line[j] == '\0')){
                        table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                        if(encoding_strlen(table_object->table[line_num][col_num], config) > table_object->cell_width[col_num])
                            table_object->cell_width[col_num] = encoding_strlen(table_object->table[line_num][col_num], config);
                        in_quotes = 0;
                    }
                    // Start of a quote
                    else if(in_quotes == 0 && line[j] == '\"'){
                        in_quotes++;
                    }
                    // End of a quote
                    else if(in_quotes == 1 && line[j] == '\"' && line[j + 1] != '\"'){
                        in_quotes++;
                    }
                    // Inline quote (quote to keep in the display)
                    else if(in_quotes == 1 && line[j] == '\"' && line[j + 1] == '\"'){
                        if(in_cell_iterator < config->cell_max_width){
                            table_object->table[line_num][col_num][in_cell_iterator] = line[j];
                            in_cell_iterator++;
                        }
                        j++;
                    }
                    // Other characters
                    else{
                        if(in_cell_iterator < config->cell_max_width){
                            if(line[j] == '\n'){
                                table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = encoding_strlen(table_object->table[line_num][col_num], config);
                                in_quotes = 0;
                            }
                            else if(line[j] == '\r' && line[j + 1] == '\n'){
                                table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = encoding_strlen(table_object->table[line_num][col_num], config);
                                in_quotes = 0;
                            }
                            else if(line[j] == '\0'){
                                table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                                table_object->cell_width[col_num] = encoding_strlen(table_object->table[line_num][col_num], config);
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
    free(line);
    fclose(f);
}

/**
 * Print the table as well as the additional informations
 */
void Print_Table(table_type table_object, config_type config, display_text_type display_text){
    char *output = NULL;
    #ifdef __linux__
    Scrollback_To_Screen_Start();
    #endif
    #ifdef _WIN64
    output = (char*)malloc(config->window_length * config->window_width * 10 * sizeof(char));
    strcpy(output, "\0");
    #endif
    Hide_Cursor(output);
    int width_of_cell_changed = 0;
    int difference = 0;
    char char_buffer[20] = " ";
    int width_counter = 0;
    // Clear_Screen_By_Scrolldown();
    int amount_of_rows_reserved_for_content_other_than_rows = 5;    // the header is not considered a row
    int max_lines_to_print = config->window_length - amount_of_rows_reserved_for_content_other_than_rows;
    int sum = 0;
    int iteration_limit = 10;
    int iterator = 0;
    // Check and set the line number by which the display should start
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
    // Change cell width in case a change was made to a cell
    if(table_object->character_highlighted != -1){
        if(table_object->active_line == -1){
            if(encoding_strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]], config) + 1 > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = encoding_strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]], config) + 1;
                width_of_cell_changed = 1;
            }
        }
        else{
            if(encoding_strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]], config) + 1 > table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]]){
                table_object->cell_width[table_object->columns_order_of_display[table_object->active_column]] = encoding_strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]], config) + 1;
                width_of_cell_changed = 1;
            }
        }
    }
    // Finds which column to print first
    sum = config->window_width + 1;
    iterator = 0;
    while(sum > config->window_width && iterator < iteration_limit){
        // Get the amount of character columns the terminal would need with the current values of first_column_printed and active_column, if it is larger than the actual terminal, it increments first_column_printed
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
        // This case scenario is when the active_column is strictly lower than the first_column_printed
        else if(sum == 1 && table_object->first_column_printed > 0){
            table_object->first_column_printed--;
        }
        iterator++;
    }
    // Finds which character to print first in selected cell
    if(table_object->character_highlighted > -1){
        // If in header
        if(table_object->active_column > -1 && table_object->active_line == -1){
            // If character highlighted is the element's next character
            if(table_object->character_highlighted == strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                // If the character highlighted is after the last character printed
                if(table_object->character_highlighted > displayed_length_to_logical_length_forward(&table_object->header[table_object->columns_order_of_display[table_object->active_column]][table_object->first_character_printed], config->focused_cell_max_width - 1, config)){
                    table_object->first_character_printed = table_object->character_highlighted - displayed_length_to_logical_length_backward(&table_object->header[table_object->columns_order_of_display[table_object->active_column]][table_object->character_highlighted], config->focused_cell_max_width, config) + 1;
                }
                else{
                    table_object->first_character_printed = 0;
                }
            }
            // If the character highlighted is before the first character printed
            else if(table_object->character_highlighted < table_object->first_character_printed){
                table_object->first_character_printed = table_object->character_highlighted;
            }
            // If the character highlighted is after the last character printed
            else if(table_object->character_highlighted > table_object->first_character_printed + displayed_length_to_logical_length_forward(&table_object->header[table_object->columns_order_of_display[table_object->active_column]][table_object->first_character_printed], config->focused_cell_max_width - 1, config)){
                table_object->first_character_printed = table_object->character_highlighted - displayed_length_to_logical_length_forward(&table_object->header[table_object->columns_order_of_display[table_object->active_column]][table_object->first_character_printed], config->focused_cell_max_width - 1, config);
            }
        }
        // If in body
        else if(table_object->active_column > -1 && table_object->active_line > -1){
            // If character highlighted is the element's next character
            if(table_object->character_highlighted == strlen(table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]])){
                // If the character highlighted is after the last character printed
                if(table_object->character_highlighted > displayed_length_to_logical_length_forward(&table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][table_object->first_character_printed], config->focused_cell_max_width - 1, config)){
                    table_object->first_character_printed = table_object->character_highlighted - displayed_length_to_logical_length_backward(&table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][table_object->character_highlighted], config->focused_cell_max_width, config) + 1;
                }
                else{
                    table_object->first_character_printed = 0;
                }
            }
            // If the character highlighted is before the first character printed
            else if(table_object->character_highlighted < table_object->first_character_printed){
                table_object->first_character_printed = table_object->character_highlighted;
            }
            // If the character highlighted is after the last character printed
            else if(table_object->character_highlighted > table_object->first_character_printed + displayed_length_to_logical_length_forward(&table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][table_object->first_character_printed], config->focused_cell_max_width - 1, config)){
                table_object->first_character_printed = table_object->character_highlighted - displayed_length_to_logical_length_forward(&table_object->table[table_object->active_line][table_object->columns_order_of_display[table_object->active_column]][table_object->first_character_printed], config->focused_cell_max_width - 1, config);
            }
        }
    }

    // Header part

    // Write table border
    width_counter = 0;
    if(table_object->table_width > 0){
        // Default_Colors(config, output);
        Header_Text_Colors(config, output);
        Border_Colors(config, output);
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
            // Default_Colors(config, output);
            Header_Text_Colors(config, output);
            Border_Colors(config, output);
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
    Default_Colors(config, output);
    while(width_counter < config->window_width){
        add_to_display_buffer(" ", output);
        width_counter++;
    }
    // Print header content
    width_counter = 0;
    if(table_object->table_width > 0){
        if(table_object->active_line == -1 && table_object->active_column == -1){                // To fix
            Selection_Content_Colors(config, output);
        }
        else{
            // Default_Colors(config, output);
            Header_Text_Colors(config, output);
            Border_Colors(config, output);
        }
        add_to_display_buffer("|", output);
        if(!(table_object->active_line == -1 && table_object->active_column == -1)){                // To fix
            Header_Text_Colors(config, output);
        }
        width_counter++;
    }
    for(int i = table_object->first_column_printed; i < table_object->table_width; i++){
        if(i == table_object->active_column){   // if header is in active column
            if(table_object->character_highlighted == -1 && (table_object->active_line == -1 || table_object->active_line == -2)){
                Selection_Content_Colors(config, output);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){   // if cell width of header fits in displayed cell
                if(table_object->active_line != -1 || (table_object->character_highlighted < (int)strlen(table_object->header[table_object->columns_order_of_display[i]]))){ // strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])
                    difference = table_object->cell_width[table_object->columns_order_of_display[i]] - encoding_strlen(table_object->header[table_object->columns_order_of_display[i]], config);
                }
                else{
                    difference = table_object->cell_width[table_object->columns_order_of_display[i]] - encoding_strlen(table_object->header[table_object->columns_order_of_display[i]], config) - 1;
                }
                // sprintf(char_buffer, "%d", (int)strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]]));
                // strcat(output, char_buffer);
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    add_to_display_buffer(" ", output);
                    width_counter++;
                }
                // strcat(output, table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < strlen(table_object->header[table_object->columns_order_of_display[i]]) && width_counter < config->window_width; j++){
                    if(table_object->active_line == -1 && j == table_object->character_highlighted){
                        Selection_Content_Colors(config, output);
                    }
                    for(int k = 0; k < length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config); k++){
                        char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j+k];
                        add_to_display_buffer(char_buffer, output);
                    }
                    if(table_object->active_line == -1 && j == table_object->character_highlighted){
                        // Default_Colors(config, output);
                        Header_Text_Colors(config, output);
                    }
                    j += length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config) - 1;
                    width_counter++;
                }
                if(table_object->active_line == -1 && table_object->character_highlighted == strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                    Selection_Content_Colors(config, output);
                    add_to_display_buffer(" ", output);
                    // Default_Colors(config, output);
                    Header_Text_Colors(config, output);
                    width_counter++;
                }
            }
            else{   // if cell width of header doesn't fit in displayed cell
                if(table_object->active_line != -1 || (table_object->character_highlighted < (int)strlen(table_object->header[table_object->columns_order_of_display[i]]))){
                    difference = config->focused_cell_max_width - encoding_strlen(table_object->header[table_object->columns_order_of_display[i]], config);
                }
                else{
                    difference = config->focused_cell_max_width - encoding_strlen(table_object->header[table_object->columns_order_of_display[i]], config) - 1;
                }
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    add_to_display_buffer(" ", output);
                    width_counter++;
                }
                if(difference < 0){ // if header width doesn't fit in displayed cell
                    // if(table_object->active_line == -1 && table_object->character_highlighted == strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                    if(table_object->active_line == -1 && table_object->character_highlighted != -1){
                        for(int j = table_object->first_character_printed; j < table_object->first_character_printed + displayed_length_to_logical_length_forward(&table_object->header[table_object->columns_order_of_display[i]][table_object->first_character_printed], config->focused_cell_max_width, config) && width_counter < config->window_width; j++){
                            if(table_object->active_line == -1 && j == table_object->character_highlighted){
                                Selection_Content_Colors(config, output);
                            }
                            for(int k = 0; k < length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config); k++){
                                char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j+k];
                                add_to_display_buffer(char_buffer, output);
                            }
                            if(table_object->active_line == -1 && j == table_object->character_highlighted){
                                // Default_Colors(config, output);
                                Header_Text_Colors(config, output);
                            }
                            j += length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config) - 1;
                            // strcat(output, table_object->header[table_object->columns_order_of_display[i]][j]);
                            width_counter++;
                        }
                    }
                    else{
                        for(int j = 0; j < displayed_length_to_logical_length_forward(&table_object->header[table_object->columns_order_of_display[i]][0], config->focused_cell_max_width, config) && width_counter < config->window_width; j++){
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
                        if(table_object->active_line == -1 && j == table_object->character_highlighted){
                            Selection_Content_Colors(config, output);
                        }
                        for(int k = 0; k < length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config); k++){
                            char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j+k];
                            add_to_display_buffer(char_buffer, output);
                        }
                        if(table_object->active_line == -1 && j == table_object->character_highlighted){
                            // Default_Colors(config, output);
                            Header_Text_Colors(config, output);
                        }
                        j += length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config) - 1;
                        width_counter++;
                    }
                }
                if(table_object->active_line == -1 && table_object->character_highlighted == strlen(table_object->header[table_object->columns_order_of_display[table_object->active_column]])){
                    Selection_Content_Colors(config, output);
                    add_to_display_buffer(" ", output);
                    // Default_Colors(config, output);
                    Header_Text_Colors(config, output);
                    // The following code is made so that the columns stay aligned well,
                    // otherwise the following lines end up misaligned one character on the left or one character on the right.
                    // It is probably due to a missing or one too much increment to the width_counter variable somewhere in the code.
                    // I looked through the code to properly fix it but I didn't find it.
                    // This piece of code isn't exactly great but it seems to fix it
                    width_counter++;
                    if(table_object->character_highlighted > config->focused_cell_max_width-1){
                        width_counter--;
                    }
                }
            }
            // Default_Colors(config, output);
            Header_Text_Colors(config, output);
        }
        else{   // if header not in active column
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                difference = table_object->cell_width[table_object->columns_order_of_display[i]] - encoding_strlen(table_object->header[table_object->columns_order_of_display[i]], config);
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    add_to_display_buffer(" ", output);
                    width_counter++;
                }
                // strcat(output, table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < strlen(table_object->header[table_object->columns_order_of_display[i]]) && width_counter < config->window_width; j++){
                    for(int k = 0; k < length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config); k++){
                        char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j+k];
                        add_to_display_buffer(char_buffer, output);
                    }
                    j += length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config) - 1;
                    width_counter++;
                }
            }
            else{
                difference = config->unfocused_cell_max_width - encoding_strlen(table_object->header[table_object->columns_order_of_display[i]], config);
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    add_to_display_buffer(" ", output);
                    width_counter++;
                }
                if(difference < 0){
                    for(int j = 0; j < displayed_length_to_logical_length_forward(&table_object->header[table_object->columns_order_of_display[i]][0], config->unfocused_cell_max_width, config) && width_counter < config->window_width; j++){
                        for(int k = 0; k < length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config); k++){
                            char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j+k];
                            add_to_display_buffer(char_buffer, output);
                        }
                        j += length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config) - 1;
                        width_counter++;
                    }
                }
                else{
                    // strcat(output, table_object->header[table_object->columns_order_of_display[i]]);
                    for(int j = 0; j < strlen(table_object->header[table_object->columns_order_of_display[i]]) && width_counter < config->window_width; j++){
                        for(int k = 0; k < length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config); k++){
                            char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j+k];
                            add_to_display_buffer(char_buffer, output);
                        }
                        j += length_of_first_character(&table_object->header[table_object->columns_order_of_display[i]][j], config) - 1;
                        width_counter++;
                    }
                }
            }
        }
        if(width_counter < config->window_width){
            if(!(table_object->active_line == -1 && table_object->active_column == -1)){                // To fix
                Header_Text_Colors(config, output);
                Border_Colors(config, output);
            }
            add_to_display_buffer("|", output);
            if(!(table_object->active_line == -1 && table_object->active_column == -1)){                // To fix
                Header_Text_Colors(config, output);
            }
            width_counter++;
        }
    }
    // if(table_object->active_line == -1 && table_object->active_column == -1){
    //     Default_Colors(config, output);
    // }
    Default_Colors(config, output);
    while(width_counter < config->window_width){
        add_to_display_buffer(" ", output);
        width_counter++;
    }
    // Write table border
    width_counter = 0;
    if(table_object->table_width > 0){
        // Default_Colors(config, output);
        Header_Text_Colors(config, output);
        Border_Colors(config, output);
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
            // Default_Colors(config, output);
            Header_Text_Colors(config, output);
            Border_Colors(config, output);
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
    Default_Colors(config, output);
    while(width_counter < config->window_width){
        add_to_display_buffer(" ", output);
        width_counter++;
    }

    // Body part

    // Print body content
    for(int i = table_object->first_line_printed; i < table_object->table_length && (i - table_object->first_line_printed) < max_lines_to_print; i++){
        width_counter = 0;
        if(i == table_object->active_line && table_object->active_column == -1){
            Selection_Content_Colors(config, output);
        }
        else{
            if((i + 1)%2 == 0){
                Even_Text_Colors(config, output);
            }
            else{
                Odd_Text_Colors(config, output);
            }
            Border_Colors(config, output);
        }
        add_to_display_buffer("|", output);
        if(!(i == table_object->active_line && table_object->active_column == -1)){
            if((i + 1)%2 == 0){
                Even_Text_Colors(config, output);
            }
            else{
                Odd_Text_Colors(config, output);
            }
        }
        width_counter++;
        for(int j = table_object->first_column_printed; j < table_object->table_width; j++){
            if(j == table_object->active_column){   // if cell in active column
                if(table_object->character_highlighted == -1 && (table_object->active_line == i || table_object->active_line == -2)){
                    Selection_Content_Colors(config, output);
                }
                if(table_object->cell_width[table_object->columns_order_of_display[j]] < config->focused_cell_max_width){   // if cell width of cell fits in displayed cell
                    if(i != table_object->active_line || (table_object->character_highlighted < (int)strlen(table_object->table[i][table_object->columns_order_of_display[j]]))){
                        difference = table_object->cell_width[table_object->columns_order_of_display[j]] - encoding_strlen(table_object->table[i][table_object->columns_order_of_display[j]], config);
                    }
                    else{
                        difference = table_object->cell_width[table_object->columns_order_of_display[j]] - encoding_strlen(table_object->table[i][table_object->columns_order_of_display[j]], config) - 1;
                    }
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        add_to_display_buffer(" ", output);
                        width_counter++;
                    }
                    // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                        if(table_object->active_line == i && k == table_object->character_highlighted){
                            Selection_Content_Colors(config, output);
                        }
                        for(int l = 0; l < length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config); l++){
                            char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k+l];
                            add_to_display_buffer(char_buffer, output);
                        }
                        if(table_object->active_line == i && k == table_object->character_highlighted){
                            if((i + 1)%2 == 0){
                                Even_Text_Colors(config, output);
                            }
                            else{
                                Odd_Text_Colors(config, output);
                            }
                        }
                        k += length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config) - 1;
                        width_counter++;
                    }
                    if(table_object->active_line == i && table_object->character_highlighted == strlen(table_object->table[i][table_object->columns_order_of_display[table_object->active_column]])){
                        Selection_Content_Colors(config, output);
                        add_to_display_buffer(" ", output);
                        if((i + 1)%2 == 0){
                            Even_Text_Colors(config, output);
                        }
                        else{
                            Odd_Text_Colors(config, output);
                        }
                        width_counter++;
                    }
                }
                else{   // if cell width of cell doesn't fit in displayed cell
                    if(i != table_object->active_line || (table_object->character_highlighted < (int)strlen(table_object->table[i][table_object->columns_order_of_display[j]]))){
                        difference = config->focused_cell_max_width - encoding_strlen(table_object->table[i][table_object->columns_order_of_display[j]], config);
                    }
                    else{
                        difference = config->focused_cell_max_width - encoding_strlen(table_object->table[i][table_object->columns_order_of_display[j]], config) - 1;
                    }
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        add_to_display_buffer(" ", output);
                        width_counter++;
                    }
                    if(difference < 0){ // if cell width doesn't fit in displayed cell
                        if(i == table_object->active_line && table_object->character_highlighted != -1){
                            for(int k = table_object->first_character_printed; k < table_object->first_character_printed + displayed_length_to_logical_length_forward(&table_object->table[i][table_object->columns_order_of_display[j]][table_object->first_character_printed], config->focused_cell_max_width, config) && width_counter < config->window_width; k++){
                                if(table_object->active_line == i && k == table_object->character_highlighted){
                                    Selection_Content_Colors(config, output);
                                }
                                for(int l = 0; l < length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config); l++){
                                    char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k+l];
                                    add_to_display_buffer(char_buffer, output);
                                }
                                if(table_object->active_line == i && k == table_object->character_highlighted){
                                    if((i + 1)%2 == 0){
                                        Even_Text_Colors(config, output);
                                    }
                                    else{
                                        Odd_Text_Colors(config, output);
                                    }
                                }
                                k += length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config) - 1;
                                // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]][k]);
                                width_counter++;
                            }
                        }
                        else{
                            for(int k = 0; k < displayed_length_to_logical_length_forward(&table_object->table[i][table_object->columns_order_of_display[j]][0], config->focused_cell_max_width, config) && width_counter < config->window_width; k++){
                                for(int l = 0; l < length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config); l++){
                                    char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k+l];
                                    add_to_display_buffer(char_buffer, output);
                                }
                                k += length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config) - 1;
                                width_counter++;
                            }
                        }
                    }
                    else{   // if cell width fits in displayed cell
                        // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                        for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                            if(table_object->active_line == i && k == table_object->character_highlighted){
                                Selection_Content_Colors(config, output);
                            }
                            for(int l = 0; l < length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config); l++){
                                char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k+l];
                                add_to_display_buffer(char_buffer, output);
                            }
                            if(table_object->active_line == i && k == table_object->character_highlighted){
                                if((i + 1)%2 == 0){
                                    Even_Text_Colors(config, output);
                                }
                                else{
                                    Odd_Text_Colors(config, output);
                                }
                            }
                            k += length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config) - 1;
                            width_counter++;
                        }
                    }
                    if(table_object->active_line == i && table_object->character_highlighted == strlen(table_object->table[i][table_object->columns_order_of_display[table_object->active_column]])){
                        Selection_Content_Colors(config, output);
                        add_to_display_buffer(" ", output);
                        // Default_Colors(config, output);
                        if((i + 1)%2 == 0){
                            Even_Text_Colors(config, output);
                        }
                        else{
                            Odd_Text_Colors(config, output);
                        }
                        // The following code is made so that the columns stay aligned well,
                        // otherwise the following lines end up misaligned one character on the left or one character on the right.
                        // It is probably due to a missing or one too much increment to the width_counter variable somewhere in the code.
                        // I looked through the code to properly fix it but I didn't find it.
                        // This piece of code isn't exactly great but it seems to fix it
                        width_counter++;
                        if(table_object->character_highlighted > config->focused_cell_max_width-1){
                            width_counter--;
                        }
                    }
                }
                // Default_Colors(config);
                if((i + 1)%2 == 0){
                    Even_Text_Colors(config, output);
                }
                else{
                    Odd_Text_Colors(config, output);
                }
            }
            else{   // if cell not in active column
                if(table_object->cell_width[table_object->columns_order_of_display[j]] < config->unfocused_cell_max_width){
                    difference = table_object->cell_width[table_object->columns_order_of_display[j]] - encoding_strlen(table_object->table[i][table_object->columns_order_of_display[j]], config);
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        add_to_display_buffer(" ", output);
                        width_counter++;
                    }
                    // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                        for(int l = 0; l < length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config); l++){
                            char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k+l];
                            add_to_display_buffer(char_buffer, output);
                        }
                        k += length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config) - 1;
                        width_counter++;
                    }
                }
                else{
                    difference = config->unfocused_cell_max_width - encoding_strlen(table_object->table[i][table_object->columns_order_of_display[j]], config);
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        add_to_display_buffer(" ", output);
                        width_counter++;
                    }
                    if(difference < 0){
                        for(int k = 0; k < displayed_length_to_logical_length_forward(&table_object->table[i][table_object->columns_order_of_display[j]][0], config->unfocused_cell_max_width, config) && width_counter < config->window_width; k++){
                            for(int l = 0; l < length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config); l++){
                                char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k+l];
                                add_to_display_buffer(char_buffer, output);
                            }
                            k += length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config) - 1;
                            width_counter++;
                        }
                    }
                    else{
                        // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                        for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                            for(int l = 0; l < length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config); l++){
                                char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k+l];
                                add_to_display_buffer(char_buffer, output);
                            }
                            k += length_of_first_character(&table_object->table[i][table_object->columns_order_of_display[j]][k], config) - 1;
                            width_counter++;
                        }
                    }
                }
            }
            if(width_counter < config->window_width){
                if(i == table_object->active_line && table_object->active_column == -1){
                    Selection_Content_Colors(config, output);
                }
                else{
                    if((i + 1)%2 == 0){
                        Even_Text_Colors(config, output);
                    }
                    else{
                        Odd_Text_Colors(config, output);
                    }
                    Border_Colors(config, output);
                }
                add_to_display_buffer("|", output);
                if(!(i == table_object->active_line && table_object->active_column == -1)){
                    if((i + 1)%2 == 0){
                        Even_Text_Colors(config, output);
                    }
                    else{
                        Odd_Text_Colors(config, output);
                    }
                }
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

    // Write table border
    width_counter = 0;
    if(table_object->table_width > 0){
        // Default_Colors(config, output);
        Odd_Text_Colors(config, output);
        Border_Colors(config, output);
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
            // Default_Colors(config, output);
            Odd_Text_Colors(config, output);
            Border_Colors(config, output);
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
    Default_Colors(config, output);
    while(width_counter < config->window_width){
        add_to_display_buffer(" ", output);
        width_counter++;
    }
    // Fill the empty lines
    for(int i = table_object->table_length; i < max_lines_to_print; i++){
        for(int j = 0; j < config->window_width; j++){
            add_to_display_buffer(" ", output);
        }
    }

    // Print comand and stuff from the very last line
    width_counter = 0;
    int digit_count_length = table_object->table_length > 9 ? (int) floor(log10(table_object->table_length)) + 1 : 2;
    int digit_count_width = table_object->table_width > 9 ? (int) floor(log10(table_object->table_width)) + 1 : 2;
    int digit_count_character = config->cell_max_width > 9 ? (int) floor(log10(config->cell_max_width)) + 1 : 2;
    int digit_active_line = table_object->active_line > 0 ? (int) floor(log10(table_object->active_line)) + 1 : (table_object->active_line < 0 ? 2 : 1);
    int digit_active_column = table_object->active_column > 0 ? (int) floor(log10(table_object->active_column)) + 1 : (table_object->active_column < 0 ? 2 : 1);
    int digit_active_character = table_object->character_highlighted > 0 ? (int) floor(log10(table_object->character_highlighted)) + 1 : (table_object->character_highlighted < 0 ? 2 : 1);
    int in_table_coord_size = digit_count_length + digit_count_width + digit_count_character + 2;
    int max_command_display_size = config->window_width - in_table_coord_size;

    if(display_text->type & BOTTOM_TEXT){
        for(int i = 0; i < strlen(display_text->text); i++){
            if(i == table_object->command_character_highlighted){
                Selection_Content_Colors(config, output);
            }
            char_buffer[0] = display_text->text[i];
            add_to_display_buffer(char_buffer, output);
            width_counter++;
            if(i == table_object->command_character_highlighted){
                Default_Colors(config, output);
            }
        }
        if(table_object->command_character_highlighted == strlen(display_text->text)){
            Selection_Content_Colors(config, output);
            add_to_display_buffer(" ", output);
            width_counter++;
            Default_Colors(config, output);
        }
    }

    // Fill the rest of the characters after the comand
    for(int i = width_counter; i < max_command_display_size; i++){
        add_to_display_buffer(" ", output);
        width_counter++;
    }

    // Print coord information
    for(int i = 0; i < digit_count_length - digit_active_line; i++){
        add_to_display_buffer(" ", output);
    }
    // itoa(table_object->active_line, char_buffer, 10);
    sprintf(char_buffer, "%d", table_object->active_line);
    add_to_display_buffer(char_buffer, output);
    add_to_display_buffer(",", output);
    // itoa(table_object->active_column, char_buffer, 10);
    sprintf(char_buffer, "%d", table_object->active_column);
    add_to_display_buffer(char_buffer, output);
    for(int i = 0; i < digit_count_width - digit_active_column; i++){
        add_to_display_buffer(" ", output);
    }
    add_to_display_buffer(",", output);
    // itoa(table_object->active_column, char_buffer, 10);
    sprintf(char_buffer, "%d", table_object->character_highlighted);
    add_to_display_buffer(char_buffer, output);
    for(int i = 0; i < digit_count_character - digit_active_character; i++){
        add_to_display_buffer(" ", output);
    }
    
    // Check later
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
    printf("\e[0m");
    free(output);
    #endif

    // FILE* debug_type_file = fopen("./col_types.txt", "w");
    // for(int i = 0; i < table_object->table_width; i++){
    //     fprintf(debug_type_file, "%d\n", table_object->type[i]);
    // }
    // fclose(debug_type_file);
}

void Save_Table(table_type table_object, config_type config){
    FILE *f = fopen(config->input_file, "w");
    if(f == NULL){
        // printf("file error\n");
        no_output_string_display("file error\n");
        return;
    }
    for(int j = 0; j < table_object->table_width; j++){
        fprintf(f, "\"");
        fprintf(f, table_object->header[table_object->columns_order_of_display[j]]);
        fprintf(f, "\"");
        if(j < table_object->table_width - 1){
            fprintf(f, "%c", config->output_separator);
        }
    }
    fprintf(f, "\n");
    for(int i = 0; i < table_object->table_length; i++){
        for(int j = 0; j < table_object->table_width; j++){
            fprintf(f, "\"");
            fprintf(f, table_object->table[i][table_object->columns_order_of_display[j]]);
            fprintf(f, "\"");
            if(j < table_object->table_width - 1){
                fprintf(f, "%c", config->output_separator);
            }
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return;
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
    free(table_object->type);
    
    free(table_object->columns_order_of_display);
    
    free(table_object->cell_width);

    for(int j = 0; j < config->commands_history_length + 1; j++){
        free(table_object->command[j]);
    }
    free(table_object->command);

    free(table_object);

    return NULL;
}