#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../lib/table.h"

void Get_File_Characteristics(table_type table_object, config_type config, char sep){
    FILE *f = fopen(config->input_file, "r");
    if(f == NULL){
        printf("file error\n");
        return;
    }
    int col_num = 1;
    int line_num = 0;
    char *line = malloc((config->file_line_max_length + 1) * sizeof(*line));
    int i = 0;
    while(!feof(f) && i < config->file_max_length){
        line = fgets(line, config->file_line_max_length + 1, f);
        if(line != NULL){
            // printf(line);
            if(i == 0){
                for(int j = 0; j < strlen(line); j++){
                    if(line[j] == sep)
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
    char sep = ',';
    Get_File_Characteristics(table_object, config, sep);
    FILE *f = fopen(config->input_file, "r");
    if(f == NULL){
        printf("file error\n");
        return;
    }
    int col_num = 0;
    int line_num = 0;
    int in_cell_iterator = 0;
    char *line = malloc((config->file_line_max_length + 1) * sizeof(*line));
    int i = 0;
    // printf("%d,%d\n", table_object->table_length, table_object->table_width);
    while(!feof(f) && i < config->file_max_length){
        line = fgets(line, config->file_line_max_length + 1, f);
        if(line != NULL){
            // printf(line);
            if(i == 0){
                table_object->header = malloc(table_object->table_width * sizeof(*table_object->header));
                for(int j = 0; j < table_object->table_width; j++){
                    table_object->header[j] = malloc((config->cell_max_width + 1) * sizeof(char));
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
                    if(line[j] == sep){
                        table_object->header[col_num][in_cell_iterator] = '\0';
                        table_object->cell_width[col_num] = strlen(table_object->header[col_num]);
                        // printf("%s\n", table_object->header[col_num]);
                        col_num++;
                        in_cell_iterator = 0;
                    }
                    else if(line[j] == '\n'){
                        table_object->header[col_num][in_cell_iterator] = '\0';
                        table_object->cell_width[col_num] = strlen(table_object->header[col_num]);
                        // printf("%s\n", table_object->header[col_num]);
                    }
                    else if(line[j] == '\0'){
                        table_object->header[col_num][in_cell_iterator] = '\0';
                        table_object->cell_width[col_num] = strlen(table_object->header[col_num]);
                        // printf("%s\n", table_object->header[col_num]);
                    }
                    else{
                        if(in_cell_iterator < config->cell_max_width){
                            table_object->header[col_num][in_cell_iterator] = line[j];
                            in_cell_iterator++;
                        }
                    }
                }
            }
            else{
                if(i == 1){
                    // Unclean way of declaring the table, to change
                    table_object->table = malloc(table_object->table_length * sizeof(char **));
                    for(int j = 0; j < table_object->table_length; j++){
                        table_object->table[j] = malloc(table_object->table_width * sizeof(char *));
                        for(int k = 0; k < table_object->table_width; k++){
                            table_object->table[j][k] = malloc((config->cell_max_width + 1) * sizeof(char));
                        }
                    }

                    // // To make a cleaner declaration but doesn't work as intended
                    // table_object->table = malloc(sizeof(char[table_object->table_length][table_object->table_width][config->cell_max_width + 1]));

                    line_num = 0;
                }
                col_num = 0;
                in_cell_iterator = 0;
                for(int j = 0; j < strlen(line) + 1; j++){  // +1 to be sure of getting a '\0' character
                    if(line[j] == sep){
                        table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                        if(strlen(table_object->table[line_num][col_num]) > table_object->cell_width[col_num])
                            table_object->cell_width[col_num] = strlen(table_object->table[line_num][col_num]);
                        // printf("%s\n", table_object->table[line_num][col_num]);
                        col_num++;
                        in_cell_iterator = 0;
                    }
                    else if(line[j] == '\n'){
                        table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                        if(strlen(table_object->table[line_num][col_num]) > table_object->cell_width[col_num])
                            table_object->cell_width[col_num] = strlen(table_object->table[line_num][col_num]);
                        // printf("%s\n", table_object->table[line_num][col_num]);
                    }
                    else if(line[j] == '\0'){
                        table_object->table[line_num][col_num][in_cell_iterator] = '\0';
                        if(strlen(table_object->table[line_num][col_num]) > table_object->cell_width[col_num])
                            table_object->cell_width[col_num] = strlen(table_object->table[line_num][col_num]);
                        // printf("%s\n", table_object->table[line_num][col_num]);
                    }
                    else{
                        if(in_cell_iterator < config->cell_max_width){
                            table_object->table[line_num][col_num][in_cell_iterator] = line[j];
                            in_cell_iterator++;
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
    printf("%d\t%d", table_object->table_length, table_object->table_width);
    free(line);
    fclose(f);
}

void Print_Table(table_type table_object, config_type config){
    int difference = 0;
    Clear_Screen();

    // Header part

    if(table_object->table_width > 0){
        Default_Colors(config);
        printf("+");
    }
    for(int i = 0; i < table_object->table_width; i++){
        if(i == table_object->active_column){
            if(table_object->active_line == -2){
                Selection_Content_Colors(config);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]]; j++){
                    printf("-");
                }
            }
            else{
                for(int j = 0; j < config->focused_cell_max_width; j++){
                    printf("-");
                }
            }
            Default_Colors(config);
        }
        else{
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]]; j++){
                    printf("-");
                }
            }
            else{
                for(int j = 0; j < config->unfocused_cell_max_width; j++){
                    printf("-");
                }
            }
        }
        printf("+");
    }
    printf("\n");
    if(table_object->table_width > 0){
        if(table_object->active_line == -1 && table_object->active_column == -1){                // To fix
            Selection_Content_Colors(config);
        }
        else{
            Default_Colors(config);
        }
        printf("|");
    }
    for(int i = 0; i < table_object->table_width; i++){
        if(i == table_object->active_column){
            if(table_object->active_line == -1 || table_object->active_line == -2){
                Selection_Content_Colors(config);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                difference = table_object->cell_width[table_object->columns_order_of_display[i]] - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < difference; j++){
                    printf(" ");
                }
                printf("%s", table_object->header[table_object->columns_order_of_display[i]]);
            }
            else{
                difference = config->focused_cell_max_width - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < difference; j++){
                    printf(" ");
                }
                if(difference < 0){
                    for(int j = 0; j < config->focused_cell_max_width; j++){
                        printf("%c", table_object->header[table_object->columns_order_of_display[i]][j]);
                    }
                }
                else{
                    printf("%s", table_object->header[table_object->columns_order_of_display[i]]);
                }
            }
            Default_Colors(config);
        }
        else{
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                difference = table_object->cell_width[table_object->columns_order_of_display[i]] - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < difference; j++){
                    printf(" ");
                }
                printf("%s", table_object->header[table_object->columns_order_of_display[i]]);
            }
            else{
                difference = config->unfocused_cell_max_width - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < difference; j++){
                    printf(" ");
                }
                if(difference < 0){
                    for(int j = 0; j < config->unfocused_cell_max_width; j++){
                        printf("%c", table_object->header[table_object->columns_order_of_display[i]][j]);
                    }
                }
                else{
                    printf("%s", table_object->header[table_object->columns_order_of_display[i]]);
                }
            }
        }
        printf("|");
    }
    if(table_object->active_line == -1 && table_object->active_column == -1){
        Default_Colors(config);
    }
    printf("\n");
    if(table_object->table_width > 0){
        Default_Colors(config);
        printf("+");
    }
    for(int i = 0; i < table_object->table_width; i++){
        if(i == table_object->active_column){
            if(table_object->active_line == -2){
                Selection_Content_Colors(config);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]]; j++){
                    printf("-");
                }
            }
            else{
                for(int j = 0; j < config->focused_cell_max_width; j++){
                    printf("-");
                }
            }
            Default_Colors(config);
        }
        else{
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]]; j++){
                    printf("-");
                }
            }
            else{
                for(int j = 0; j < config->unfocused_cell_max_width; j++){
                    printf("-");
                }
            }
        }
        printf("+");
    }
    printf("\n");

    // Body part

    int max_lines_to_print = config->window_length - 5;
    for(int i = table_object->first_line_printed; i < table_object->table_length && (i - table_object->first_line_printed) < max_lines_to_print; i++){
        if((i + 1)%2 == 0){
            Even_Line_Colors(config);
        }
        else{
            Odd_Line_Colors(config);
        }
        if(i == table_object->active_line && table_object->active_column == -1){
            Selection_Content_Colors(config);
        }
        printf("|");
        for(int j = 0; j < table_object->table_width; j++){
            if(j == table_object->active_column){
                if(table_object->active_line == i || table_object->active_line == -2){
                    Selection_Content_Colors(config);
                }
                if(table_object->cell_width[table_object->columns_order_of_display[j]] < config->focused_cell_max_width){
                    difference = table_object->cell_width[table_object->columns_order_of_display[j]] - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < difference; k++){
                        printf(" ");
                    }
                    printf("%s", table_object->table[i][table_object->columns_order_of_display[j]]);
                }
                else{
                    difference = config->focused_cell_max_width - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < difference; k++){
                        printf(" ");
                    }
                    if(difference < 0){
                        for(int k = 0; k < config->focused_cell_max_width; k++){
                            printf("%c", table_object->table[i][table_object->columns_order_of_display[j]][k]);
                        }
                    }
                    else{
                        printf("%s", table_object->table[i][table_object->columns_order_of_display[j]]);
                    }
                }
                // Default_Colors(config);
                if((i + 1)%2 == 0){
                    Even_Line_Colors(config);
                }
                else{
                    Odd_Line_Colors(config);
                }
            }
            else{
                if(table_object->cell_width[table_object->columns_order_of_display[j]] < config->unfocused_cell_max_width){
                    difference = table_object->cell_width[table_object->columns_order_of_display[j]] - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < difference; k++){
                        printf(" ");
                    }
                    printf("%s", table_object->table[i][table_object->columns_order_of_display[j]]);
                }
                else{
                    difference = config->unfocused_cell_max_width - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < difference; k++){
                        printf(" ");
                    }
                    if(difference < 0){
                        for(int k = 0; k < config->unfocused_cell_max_width; k++){
                            printf("%c", table_object->table[i][table_object->columns_order_of_display[j]][k]);
                        }
                    }
                    else{
                        printf("%s", table_object->table[i][table_object->columns_order_of_display[j]]);
                    }
                }
            }
            printf("|");
        }
        if(i == table_object->active_line && table_object->active_column == -1){
            Default_Colors(config);
        }
        printf("\n");
    }

    // Last line

    if(table_object->table_width > 0){
        Default_Colors(config);
        printf("+");
    }
    for(int i = 0; i < table_object->table_width; i++){
        if(i == table_object->active_column){
            if(table_object->active_line == -2){
                Selection_Content_Colors(config);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]]; j++){
                    printf("-");
                }
            }
            else{
                for(int j = 0; j < config->focused_cell_max_width; j++){
                    printf("-");
                }
            }
            Default_Colors(config);
        }
        else{
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]]; j++){
                    printf("-");
                }
            }
            else{
                for(int j = 0; j < config->unfocused_cell_max_width; j++){
                    printf("-");
                }
            }
        }
        printf("+");
    }
    printf("\n : ");
}

void S_Print_Table(table_type table_object, config_type config){
    char *output = (char*)malloc(config->window_length * config->window_width * 2 * sizeof(char));
    strcpy(output, "\0");
    // output[0] = "\0";
    int difference = 0;
    char char_buffer[2] = " ";
    int width_counter = 0;
    // Clear_Screen();
    int amount_of_rows_reserved_for_content_other_than_rows = 5;    // the header is not considered a row
    int max_lines_to_print = config->window_length - amount_of_rows_reserved_for_content_other_than_rows;
    int sum = config->window_length + 1;
    int iteration_limit = 10;
    int iterator = 0;
    if(table_object->active_line < table_object->first_line_printed && table_object->active_line >= 0){
        table_object->first_line_printed = table_object->active_line;
    }
    else if(table_object->active_line > table_object->first_line_printed + max_lines_to_print - 1){
        table_object->first_line_printed = table_object->active_line - max_lines_to_print + 1;
    }
    sum = config->window_width + 1;
    iteration_limit = 10;
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

    // Header part

    if(table_object->table_width > 0){
        S_Default_Colors(config, output);
        strcat(output, "+");
        width_counter++;
    }
    for(int i = table_object->first_column_printed; i < table_object->table_width; i++){
        if(i == table_object->active_column){
            if(table_object->active_line == -2){
                S_Selection_Content_Colors(config, output);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->focused_cell_max_width && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
            S_Default_Colors(config, output);
        }
        else{
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->unfocused_cell_max_width && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
        }
        if(width_counter < config->window_width){
            strcat(output, "+");
            width_counter++;
        }
    }
    strcat(output, "\n");
    width_counter = 0;
    if(table_object->table_width > 0){
        if(table_object->active_line == -1 && table_object->active_column == -1){                // To fix
            S_Selection_Content_Colors(config, output);
        }
        else{
            S_Default_Colors(config, output);
        }
        strcat(output, "|");
        width_counter++;
    }
    for(int i = table_object->first_column_printed; i < table_object->table_width; i++){
        if(i == table_object->active_column){
            if(table_object->active_line == -1 || table_object->active_line == -2){
                S_Selection_Content_Colors(config, output);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                difference = table_object->cell_width[table_object->columns_order_of_display[i]] - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    strcat(output, " ");
                    width_counter++;
                }
                // strcat(output, table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < strlen(table_object->header[table_object->columns_order_of_display[i]]) && width_counter < config->window_width; j++){
                    char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                    strcat(output, char_buffer);
                    width_counter++;
                }
            }
            else{
                difference = config->focused_cell_max_width - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    strcat(output, " ");
                    width_counter++;
                }
                if(difference < 0){
                    for(int j = 0; j < config->focused_cell_max_width && width_counter < config->window_width; j++){
                        char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                        strcat(output, char_buffer);
                        // strcat(output, table_object->header[table_object->columns_order_of_display[i]][j]);
                        width_counter++;
                    }
                }
                else{
                    // strcat(output, table_object->header[table_object->columns_order_of_display[i]]);
                    for(int j = 0; j < strlen(table_object->header[table_object->columns_order_of_display[i]]) && width_counter < config->window_width; j++){
                        char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                        strcat(output, char_buffer);
                        width_counter++;
                    }
                }
            }
            S_Default_Colors(config, output);
        }
        else{
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                difference = table_object->cell_width[table_object->columns_order_of_display[i]] - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    strcat(output, " ");
                    width_counter++;
                }
                // strcat(output, table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < strlen(table_object->header[table_object->columns_order_of_display[i]]) && width_counter < config->window_width; j++){
                    char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                    strcat(output, char_buffer);
                    width_counter++;
                }
            }
            else{
                difference = config->unfocused_cell_max_width - strlen(table_object->header[table_object->columns_order_of_display[i]]);
                for(int j = 0; j < difference && width_counter < config->window_width; j++){
                    strcat(output, " ");
                    width_counter++;
                }
                if(difference < 0){
                    for(int j = 0; j < config->unfocused_cell_max_width && width_counter < config->window_width; j++){
                        char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                        strcat(output, char_buffer);
                        // strcat(output, table_object->header[table_object->columns_order_of_display[i]][j]);
                        width_counter++;
                    }
                }
                else{
                    // strcat(output, table_object->header[table_object->columns_order_of_display[i]]);
                    for(int j = 0; j < strlen(table_object->header[table_object->columns_order_of_display[i]]) && width_counter < config->window_width; j++){
                        char_buffer[0] = table_object->header[table_object->columns_order_of_display[i]][j];
                        strcat(output, char_buffer);
                        width_counter++;
                    }
                }
            }
        }
        if(width_counter < config->window_width){
            strcat(output, "|");
            width_counter++;
        }
    }
    if(table_object->active_line == -1 && table_object->active_column == -1){
        S_Default_Colors(config, output);
    }
    strcat(output, "\n");
    width_counter = 0;
    if(table_object->table_width > 0){
        S_Default_Colors(config, output);
        strcat(output, "+");
        width_counter++;
    }
    for(int i = table_object->first_column_printed; i < table_object->table_width; i++){
        if(i == table_object->active_column){
            if(table_object->active_line == -2){
                S_Selection_Content_Colors(config, output);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->focused_cell_max_width && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
            S_Default_Colors(config, output);
        }
        else{
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->unfocused_cell_max_width && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
        }
        if(width_counter < config->window_width){
            strcat(output, "+");
            width_counter++;
        }
    }
    strcat(output, "\n");

    // Body part

    for(int i = table_object->first_line_printed; i < table_object->table_length && (i - table_object->first_line_printed) < max_lines_to_print; i++){
        width_counter = 0;
        if((i + 1)%2 == 0){
            S_Even_Line_Colors(config, output);
        }
        else{
            S_Odd_Line_Colors(config, output);
        }
        if(i == table_object->active_line && table_object->active_column == -1){
            S_Selection_Content_Colors(config, output);
        }
        strcat(output, "|");
        width_counter++;
        for(int j = table_object->first_column_printed; j < table_object->table_width; j++){
            if(j == table_object->active_column){
                if(table_object->active_line == i || table_object->active_line == -2){
                    S_Selection_Content_Colors(config, output);
                }
                if(table_object->cell_width[table_object->columns_order_of_display[j]] < config->focused_cell_max_width){
                    difference = table_object->cell_width[table_object->columns_order_of_display[j]] - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        strcat(output, " ");
                        width_counter++;
                    }
                    // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                        char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                        strcat(output, char_buffer);
                        width_counter++;
                    }
                }
                else{
                    difference = config->focused_cell_max_width - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        strcat(output, " ");
                        width_counter++;
                    }
                    if(difference < 0){
                        for(int k = 0; k < config->focused_cell_max_width && width_counter < config->window_width; k++){
                            char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                            strcat(output, char_buffer);
                            // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]][k]);
                            width_counter++;
                        }
                    }
                    else{
                        // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                        for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                            char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                            strcat(output, char_buffer);
                            width_counter++;
                        }
                    }
                }
                // Default_Colors(config);
                if((i + 1)%2 == 0){
                    S_Even_Line_Colors(config, output);
                }
                else{
                    S_Odd_Line_Colors(config, output);
                }
            }
            else{
                if(table_object->cell_width[table_object->columns_order_of_display[j]] < config->unfocused_cell_max_width){
                    difference = table_object->cell_width[table_object->columns_order_of_display[j]] - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        strcat(output, " ");
                        width_counter++;
                    }
                    // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                        char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                        strcat(output, char_buffer);
                        width_counter++;
                    }
                }
                else{
                    difference = config->unfocused_cell_max_width - strlen(table_object->table[i][table_object->columns_order_of_display[j]]);
                    for(int k = 0; k < difference && width_counter < config->window_width; k++){
                        strcat(output, " ");
                        width_counter++;
                    }
                    if(difference < 0){
                        for(int k = 0; k < config->unfocused_cell_max_width && width_counter < config->window_width; k++){
                            char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                            strcat(output, char_buffer);
                            // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]][k]);
                            width_counter++;
                        }
                    }
                    else{
                        // strcat(output, table_object->table[i][table_object->columns_order_of_display[j]]);
                        for(int k = 0; k < strlen(table_object->table[i][table_object->columns_order_of_display[j]]) && width_counter < config->window_width; k++){
                            char_buffer[0] = table_object->table[i][table_object->columns_order_of_display[j]][k];
                            strcat(output, char_buffer);
                            width_counter++;
                        }
                    }
                }
            }
            if(width_counter < config->window_width){
                strcat(output, "|");
                width_counter++;
            }
        }
        if(i == table_object->active_line && table_object->active_column == -1){
            S_Default_Colors(config, output);
        }
        strcat(output, "\n");
    }

    // Last line

    width_counter = 0;
    if(table_object->table_width > 0){
        S_Default_Colors(config, output);
        strcat(output, "+");
        width_counter++;
    }
    for(int i = table_object->first_column_printed; i < table_object->table_width; i++){
        if(i == table_object->active_column){
            if(table_object->active_line == -2){
                S_Selection_Content_Colors(config, output);
            }
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->focused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->focused_cell_max_width && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
            S_Default_Colors(config, output);
        }
        else{
            if(table_object->cell_width[table_object->columns_order_of_display[i]] < config->unfocused_cell_max_width){
                for(int j = 0; j < table_object->cell_width[table_object->columns_order_of_display[i]] && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
            else{
                for(int j = 0; j < config->unfocused_cell_max_width && width_counter < config->window_width; j++){
                    strcat(output, "-");
                    width_counter++;
                }
            }
        }
        if(width_counter < config->window_width){
            strcat(output, "+");
            width_counter++;
        }
    }
    for(int i= table_object->table_length; i < max_lines_to_print; i++){
        strcat(output, "\n");
    }
    strcat(output, "\n");
    strcat(output, table_object->command);

    Clear_Screen();
    printf("%s", output);
    free(output);
}

void Update_Cell_Width_By_Column(table_type table_object, int displayed_column_id){
    table_object->cell_width[table_object->columns_order_of_display[displayed_column_id]] = strlen(table_object->header[table_object->columns_order_of_display[displayed_column_id]]);
    for(int i = 0; i < table_object->table_length; i++){
        if(strlen(table_object->table[i][table_object->columns_order_of_display[displayed_column_id]]) > table_object->cell_width[table_object->columns_order_of_display[displayed_column_id]]){
            table_object->cell_width[table_object->columns_order_of_display[displayed_column_id]] = strlen(table_object->table[i][table_object->columns_order_of_display[displayed_column_id]]);
        }
    }
}

table_type Free_Table_Object(table_type table_object){
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

    free(table_object);

    return NULL;
}