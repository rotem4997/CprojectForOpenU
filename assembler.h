#ifndef ASSEMBLER_H

#define ASSEMBLER_H


#define MEMORY_START 100 /** The IC counter start */
#define EMPTY_WORD 0 /** Empty Word */
#define EXTERNAL_DEFAULT_ADDRESS 0 /** Defult Address */
#define NOT_FOUND -1
#define NO_ERROR 0
#define ERROR 1
#define SEPARATOR "." /** . Separator */
#define COMMA ',' /** Comma */
#define EXPANDED_EXT ".am" /** .am Extantion */

#define LINE_LENGTH 81 /** maximum chars per line */
#define MINIMUM_LABEL_LENGTH_WITH_COLON 2 /** : */
#define MINIMUM_LABEL_LENGTH_WITHOUT_COLON 1 /** : */
#define LABEL_LENGTH 30 /** maximum chars per label */

#define MAX_COMMAND_LENGTH 4 /** maximum number of characters in a command */
#define MIN_COMMAND_LENGTH 3 /** minimum number of characters in a command */

#define REGISTER_LENGTH 2 /** a register's name contains 2 characters */
#define MIN_REGISTER 0 /** r0 is the first register */
#define MAX_REGISTER 7 /** r7 is the last register */

#define MAX_EXTENSION_LENGTH 5

#define BASE32_SEQUENCE_LENGTH 3 /** A base32 sequence of a word consists of 2 digits (and '\0' ending) */

#define NUM_DIRECTIVES 5 /** number of existing directives*/
#define NUM_COMMANDS 16 /** number of existing commands */

#define FIRST_STRUCT_FIELD 1 /** Index of first struct field */
#define SECOND_STRUCT_FIELD 2 /** Index of second struct field */

#define BITS_IN_WORD 10 /** Bit-related Word */
#define BITS_IN_OPCODE 4 /** Bit-related Opcode */
#define BITS_IN_METHOD 2 /** Bit-related Method */
#define BITS_IN_ARE 2 /** Bit-related A,R,E */
#define BITS_IN_REGISTER 4 /** Bit-related Register */
#define BITS_IN_ADDRESS 8 /** Bit-related Address */

#define SRC_METHOD_START_POS 4 /** Addressing types bits location in the source first word of a command */
#define SRC_METHOD_END_POS 5 /** Addressing types bits location in the source end word of a command */
#define DEST_METHOD_START_POS 2 /** Addressing types bits location in the destination first word of a command */
#define DEST_METHOD_END_POS 3 /** Addressing types bits location in the destionation end word of a command */

#define MACHINE_RAM 256

/** Directives types */
enum directives {
    DATA, STRING, STRUCT, ENTRY, EXTERN, UNKNOWN_TYPE
    }; 

/** Commands ordered by their Opcode */
enum commands {
    MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP, UNKNOWN_COMMAND
    }; 

/** List of all errors */
enum errors {
    SYNTAX_ERR = 1, LABEL_ALREADY_EXISTS, LABEL_TOO_LONG, LABEL_INVALID_FIRST_CHAR, LABEL_ONLY_ALPHANUMERIC,
    LABEL_CANT_BE_COMMAND, LABEL_ONLY, LABEL_CANT_BE_REGISTER,
    DIRECTIVE_NO_PARAMS, DIRECTIVE_INVALID_NUM_PARAMS, DATA_COMMAS_IN_A_ROW, DATA_EXPECTED_NUM,
    DATA_EXPECTED_COMMA_AFTER_NUM, DATA_UNEXPECTED_COMMA,
    STRING_TOO_MANY_OPERANDS, STRING_OPERAND_NOT_VALID, STRUCT_EXPECTED_STRING,
    STRUCT_INVALID_STRING, EXPECTED_COMMA_BETWEEN_OPERANDS, STRUCT_INVALID_NUM, STRUCT_TOO_MANY_OPERANDS,
    EXTERN_NO_LABEL, EXTERN_INVALID_LABEL, EXTERN_TOO_MANY_OPERANDS,
    COMMAND_NOT_FOUND, COMMAND_UNEXPECTED_CHAR, COMMAND_TOO_MANY_OPERANDS,
    COMMAND_INVALID_METHOD, COMMAND_INVALID_NUMBER_OF_OPERANDS, COMMAND_INVALID_OPERANDS_METHODS,
    ENTRY_LABEL_DOES_NOT_EXIST, ENTRY_CANT_BE_EXTERN, COMMAND_LABEL_DOES_NOT_EXIST,
    CANNOT_OPEN_FILE,ER_EMPTY_MACRO
};


enum {NO_COLON, COLON}; /** Check if label should contain a colon */

/** Addressing types */
enum methods {
    METHOD_IMMEDIATE, METHOD_DIRECT, METHOD_STRUCT, METHOD_REGISTER, METHOD_UNKNOWN
    }; 

/** A,R,E modes */
enum ARE {
    ABSOLUTE, EXTERNAL, RELOCATABLE
    }; 

/** Types of files extantions */
enum filetypes {
    FILE_INPUT, FILE_OBJECT, FILE_ENTRY, FILE_EXTERN
    }; 

#endif