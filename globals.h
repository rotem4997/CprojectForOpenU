#ifndef _GLOBALS_H
#define _GLOBALS_H
#include "stdbool.h"

#define MEMORY_MAX_SIZE 256 /* memory size */
#define NUM_OF_REGISTERS 8 /* number of registers */
#define NUM_OF_OPCODE 16 /* number of op code */

#define INT8_MIN -127 /* min value number */
#define INT8_MAX 128 /* max value number */
#define ASCII_MAX_LENGTH_VALUE 32 /* max length of ascii char value* /

/** Instructions type */
typedef enum instruction 
{
        DATA, 
        STRING, 
        ENTRY,
        STRUCT,
        EXTERNAL,
        NONE_INST
} instruction;

#define LABEL 30 /* Max length of the label */
#define IC_INIT_VALUE 100 /* Initial IC Value */
#define MAX_LINE_LENGTH 80 /* Maximum length of a source of a single line */

#define END_OF_MACRO "endmacro"
#define START_OF_MACRO "macro"


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

typedef struct data_word {
    unsigned int are : 3;
    signed int data: 16;
    char *symbol_need_to_be_filled;
} data_word;

typedef struct op_code_word {
    unsigned int op_code: 16;
} op_code_word;

typedef struct word {
    int address;
    int source_operand: 2;
    int source_addressing_types: 4;
    int destination_operand: 2;
    int destination_addressing_types: 4;
    data_word *dlw;
    op_code_word *ocw;
    struct word *next;
} word;

 
typedef struct entry {
    char symbol_id[ASCII_MAX_LENGTH_VALUE];
    bool checked;
    struct entry *next;
} entry;

typedef struct external {
    char symbol_name[ASCII_MAX_LENGTH_VALUE];
    struct external * next;
} external;


typedef struct machine_instruction {
    char *name;
    int opcode;
    int amount_of_operands;
} machine_instruction;

typedef struct machine_instruction_line {
    machine_instruction *mi;
    char *source_operand;
    char *destination_operand;
} machine_instruction_line;

typedef struct macro{
    char *macro_id;
    char *macro_content;
    struct macro *next;
}macro;


#endif
