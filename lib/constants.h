#ifndef __CONSTANTS__

#define __CONSTANTS__

#define REGULAR_STATE 0
#define MOVE_STATE 1
#define COMMAND_STATE 2
#define EDIT_STATE 3
#define VALIDATION_STATE 4  // Yes or No question to answer with 'y' or 'n'
#define QUESTION_STATE 5    // To ask a question to the user to answer with a string

#define COLUMN_TYPE_INTEGER 0b00000001
#define COLUMN_TYPE_DECIMAL 0b00000010
#define COLUMN_TYPE_DATE    0b00000100
#define COLUMN_TYPE_STRING  0b00001000

#endif