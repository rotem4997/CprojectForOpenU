#ifndef _GLOBALS_H
#define _GLOBALS_H

#define MEMORY_MAX_SIZE 256 /* memory size */
#define NUM_OF_REGISTERS 8 /* number of registers */
#define NUM_OF_OPCODE 16 /* number of op code */

#define INT8_MIN -127 /* min value number */
#define INT8_MAX 128 /* max value number */
#define ASCII_MAX_LENGTH_VALUE 32 /* max length of ascii char value* /

/** Instructions type */
#define DATA = ".data"
#define STRING = ".string"
#define ENTRY = ".entry"
#define STRUCT = ".struct"
#define EXTERNAL = ".extern"

#define LABEL 30 /* Max length of the label */
#define IC_INIT_VALUE 100 /* Initial IC Value */
#define MAX_LINE_LENGTH 80 /* Maximum length of a source of a single line */


/** Boolean definition true or false */
typedef enum {false, true} bool;

/**symbol declration*/
typedef struct char_symbols
{
    char symbol_id[ASCII_MAX_LENGTH_VALUE];
    int value;

    bool is_data;
    bool is_string;
    bool is_entry;
    bool is_struct;
    bool is_external;

    struct char_symbols * next
    
} char_symbols;

/**Unique 32 base ASCII array*/

char ASCII_symbol[ASCII_MAX_LENGTH_VALUE];
ASCII_symbol[0] = '!';
ASCII_symbol[1] = '@';
ASCII_symbol[2] = '#';
ASCII_symbol[3] = '$';
ASCII_symbol[4] = '%';
ASCII_symbol[5] = '^';
ASCII_symbol[6] = '&';
ASCII_symbol[7] = '*';
ASCII_symbol[8] = '<';
ASCII_symbol[9] = '>';
ASCII_symbol[10] = 'a';
ASCII_symbol[11] = 'b';
ASCII_symbol[12] = 'c';
ASCII_symbol[13] = 'd';
ASCII_symbol[14] = 'e';
ASCII_symbol[15] = 'f';
ASCII_symbol[16] = 'g';
ASCII_symbol[17] = 'h';
ASCII_symbol[18] = 'i';
ASCII_symbol[19] = 'j';
ASCII_symbol[20] = 'k';
ASCII_symbol[21] = 'l';
ASCII_symbol[22] = 'm';
ASCII_symbol[23] = 'n';
ASCII_symbol[24] = 'o';
ASCII_symbol[25] = 'p';
ASCII_symbol[26] = 'q';
ASCII_symbol[27] = 'r';
ASCII_symbol[28] = 's';
ASCII_symbol[29] = 't';
ASCII_symbol[30] = 'u';
ASCII_symbol[31] = 'v';


/** Addressing types */

typedef enum addressing_types {
    IMMEDIATE = 0,
    DIRECT = 1,
    RELATIVE = 2,
    REGISTER = 3,
    NONE = -1
} addressing_types;


/* PC commands && opcode */
typedef enum PC_Commands {

    /* ARE */
    ABS_OP = 00, /*Absolute*/
    EXT_OP = 01, /*External*/
    REL_OP = 10, /*Relocatable*/
    /* end of ARE */

    /* First Group */
    MOV_OP = 0,
    CMP_OP = 1,
    ADD_OP = 2,
    SUB_OP = 3,
    LEA_OP = 6,
    /* END OF FIRST GROUP */

    /*SECOND GROUP */

    NOT_OP = 4,
    CLR_OP = 5,
    INC_OP = 7,
    DEC_OP = 8,
    JMP_OP = 9,
    BNE_OP = 10,
    GET_OP = 11,
    PRN_OP = 12,
    JSR_OP = 13,
    /* END OF SECOND GROUP */

    /*THIRD GROUP*/
    RTS_OP = 14,
    HLT_OP = 15,
    /* END OF THIRD GROUP */

    /* FAILD/ERROR */
    NONE_OP = -1

} PC_Commands;

/** Registers */
typedef enum registers {
    r0 = 0,
    r1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7,
    /** reg not found */
    NONE_REG = -1
} registers;

/** data decleration */
typedef struct data
{
    int dc;
    int value;
    struct data *next;
} data;

/** string decleration */
typedef struct string
{
    int dc;
    char temp_str[MAX_LINE_LENGTH];
    int length;
    struct string *next
} string;

/** Single source line */
typedef struct line_info {
    long line_number;
    char *file_name;
    char *content;
} line_info;

#endif