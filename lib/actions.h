#ifndef __ACTIONS__

#define __ACTIONS__

#include "../lib/table.h"

void set_character_highlighted_to_last_character(table_type table_object);
void reset_character_highlighted_to_default_value(table_type table_object);

int on_selection_go_up(table_type table_object);
int on_selection_go_down(table_type table_object);
int on_selection_go_left(table_type table_object);
int on_selection_go_right(table_type table_object);

int on_move_go_up(table_type table_object, char *string_buffer);
int on_move_go_down(table_type table_object, char *string_buffer);
int on_move_go_left(table_type table_object, char *string_buffer);
int on_move_go_right(table_type table_object, char *string_buffer);

void delete_command(table_type table_object);

int on_command_go_up(table_type table_object, int commands_history_length);
int on_command_go_down(table_type table_object);
int on_command_go_left(table_type table_object);
int on_command_go_right(table_type table_object);

int execute_command(table_type table_object, config_type config, int commands_history_length);

int on_command_backspace(table_type table_object);
int on_command_characters(table_type table_object, int command_string_size, char c);

int on_edit_go_left(table_type table_object);
int on_edit_go_right(table_type table_object);
int on_edit_backspace(table_type table_object);
int on_edit_characters(table_type table_object, int cell_max_width, char c);

#endif