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
typedef enum booleans 
{
    FALSE = 0,
    TRUE = 1
} booleans;

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

    struct char_symbols *next;
    
} char_symbols;

/**Unique 32 base ASCII array*/

char ASCII_symbol[ASCII_MAX_LENGTH_VALUE]
{
'!'
,'@'
,'#'
,'$'
,'%'
,'^'
,'&'
,'*'
,'<'
,'>'
,'a'
,'b'
,'c'
,'d'
,'e'
,'f'
,'g'
,'h'
,'i'
,'j'
,'k'
,'l'
,'m'
,'n'
,'o'
,'p'
,'q'
,'r'
,'s'
,'t'
,'u'
,'v'
};

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
    struct string *next;
} string;

/** Single source line */
typedef struct line_info {
    long line_number;
    char *file_name;
    char *content;
} line_info;

typedef struct status { /* file status */
    char *file_name;
    int line_number;
    bool errors_flag;
} status;

typedef struct word {
    int address;
    op_code_word *ocw;
    funct_details_word *fdw;
    data_word *dlw;
    struct word *next;
} word;


typedef struct entry {
    char symbol_id[SYMBOL_MAX_LENGTH];
    bool checked;
    struct entry *next;
} entry;

typedef struct external {
    char symbol_name[SYMBOL_MAX_LENGTH];
    struct external * next;
} external;

typedef struct data_word {
    unsigned int are : 3;
    signed int data: 16;
    char *symbol_need_to_be_filled;
} data_word;

#endif
