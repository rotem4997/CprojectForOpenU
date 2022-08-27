
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "ext_vars.h"
#include "prototypes.h"
#include "utils.h"

/** First Pass function */
void first_pass(FILE *fp)
{
    char line[LINE_LENGTH]; /** Line number*/
    int line_num = 1; /** Start of Line number*/

    /** Initializing IC, DC*/
    ic = 0;
    dc = 0;

    while(fgets(line, LINE_LENGTH, fp) != NULL) /** Read lines to EOF */
    {
        /** Error Handling*/
        err = NO_ERROR; 
        if(!ignore(line)) 
            read_line(line);
        if(is_error()) {
            was_error = TRUE; 
            write_error(line_num);
        }
        line_num++;
    }
    
    offset_addresses(symbols_table, MEMORY_START, FALSE); /** Symbols start from 100  */
    offset_addresses(symbols_table, ic + MEMORY_START, TRUE); /** Data symbols start fron 100+ IC */
}

/** Analyze line from file and extract its information */
void read_line(char *line)
{
    /** Initializing for the type of the directive/command */
    int dir_type = UNKNOWN_TYPE;
    int command_type = UNKNOWN_COMMAND;

    boolean label = FALSE; /** Set label to false. will be true if the label exists */
    labelPtr label_node = NULL; /** Var in case label is created */
    char current_token[LINE_LENGTH]; /** Current token lenght to analyze */
    
    line = skip_spaces(line); /** Skips whitepsace */
    if(end_of_line(line)) return; 
    if(!isalpha(*line) && *line != '.') { /** First character must be a letter or a dot */
        err = SYNTAX_ERR;
        return;
    }

    copy_token(current_token, line); /** Copy label That separated with whitespace */
    if(is_label(current_token, COLON)) { /** Check if the first token is a label with a colon) */
        label = TRUE;
        label_node = add_label(&symbols_table, current_token, 0, FALSE, FALSE); /** Adding label to the symbols table */
        if(label_node == NULL)
            return;
        line = next_token(line); /** Skipping to the next token */
        if(end_of_line(line))
        {
            err = LABEL_ONLY; 
            return;
        }
        copy_token(current_token, line); /** Proceed to next token */
    } 

    if(is_error())
        return;

    if((dir_type = find_directive(current_token)) != NOT_FOUND) /** Detecting directive type */
    {
        if(label)
        {
            if(dir_type == EXTERN || dir_type == ENTRY) { /** Ignore creation of label before .entry/.extern */
                delete_label(&symbols_table, label_node->name);
                label = FALSE;
            }
            else
                label_node -> address = dc; 
        }
        line = next_token(line);
        handle_directive(dir_type, line);
    }

    else if ((command_type = find_command(current_token)) != NOT_FOUND) /** Detecting command type */
    {
        if(label)
        {
            
            label_node -> inActionStatement = TRUE; 
            label_node -> address = ic;
        }
        line = next_token(line);
        handle_command(command_type, line);
    }

    else
    {
        err = COMMAND_NOT_FOUND; 
    }

}

/** Handles directives */
int handle_directive(int type, char *line)
{
    if(line == NULL || end_of_line(line)) /** Check if directives have a parameter */
    {
        err = DIRECTIVE_NO_PARAMS;
        return ERROR;
    }

    switch (type)
    {
        case DATA:
            return handle_data_directive(line);

        case STRING:
            return handle_string_directive(line);

        case STRUCT:
            return handle_struct_directive(line);

        case ENTRY:
            if(!end_of_line(next_token(line))) /** If there's a next token */
            {
                err = DIRECTIVE_INVALID_NUM_PARAMS;
                return ERROR;
            }
            break;

        case EXTERN:
            return handle_extern_directive(line);
    }
    return NO_ERROR;
}

/** Analyzes a command given and detect the addressing types to the instructions memory. */
int handle_command(int type, char *line)
{
    boolean is_first = FALSE, is_second = FALSE; /** Check wich operands were received */
    int first_method, second_method; /** Holds the addressing methods of the operands */
    char first_op[20], second_op[20]; /** Strings hold the operands */

    line = next_list_token(first_op, line);
    if(!end_of_line(first_op)) /** First operand is not empty */
    {
        is_first = TRUE;
        line = next_list_token(second_op, line);
        if(!end_of_line(second_op)) /** Second operand is not empty */
        {
            if(second_op[0] != COMMA) 
            {
                err = COMMAND_UNEXPECTED_CHAR;
                return ERROR;
            }

            else
            {
                line = next_list_token(second_op, line); 
                if(end_of_line(second_op)) 
                {
                    err = COMMAND_UNEXPECTED_CHAR;
                    return ERROR;
                }
                is_second = TRUE; 
            }
        }
    }
    line = skip_spaces(line);
    if(!end_of_line(line)) /** Check If line continues after two operands */
    {
        err = COMMAND_TOO_MANY_OPERANDS;
        return ERROR;
    }

    if(is_first)
        first_method = detect_method(first_op); 
    if(is_second)
        second_method = detect_method(second_op); 

    if(!is_error())
    {
        if(command_accept_num_operands(type, is_first, is_second)) 
        {
            if(command_accept_methods(type, first_method, second_method)) 
            {
                /** First word of the command to memory and increase ic */
                encode_to_instructions(build_first_word(type, is_first, is_second, first_method, second_method));
                ic += calculate_command_num_additional_words(is_first, is_second, first_method, second_method);
            }

            else
            {
                err = COMMAND_INVALID_OPERANDS_METHODS;
                return ERROR;
            }
        }
        else
        {
            err = COMMAND_INVALID_NUMBER_OF_OPERANDS;
            return ERROR;
        }
    }

    return NO_ERROR;
}


/** handles a .string directive */
int handle_string_directive(char *line)
{
    char token[LINE_LENGTH];

    line = next_token_string(token, line);
    if(!end_of_line(token) && is_string(token)) { /** Check if token exists */
        if(end_of_line(line))
        {
            /** Trimming quotation marks */
            token[strlen(token) - 1] = '\0';
            write_string_to_data(token + 1);
        }

        else 
        {
            err = STRING_TOO_MANY_OPERANDS;
            return ERROR;
        }

    }

    else 
    {
        err = STRING_OPERAND_NOT_VALID;
        return ERROR;
    }

    return NO_ERROR;
}

/** Analyzing  a .struct directive */
int handle_struct_directive(char *line)
{
    char token[LINE_LENGTH];
    line = next_list_token(token, line); /** First token */

    if(!end_of_line(token) && is_number(token)) /** First token is a number */
    {
        write_num_to_data(atoi(token)); /**Write token to data */
        line = next_list_token(token, line); 

        if(!end_of_line(token) && *token == COMMA) { 
            line = next_token_string(token, line); /* Get next token */
            if(!end_of_line(token)) {
                if (is_string(token)) {
                    /** Trimming the ""*/
                    token[strlen(token) - 1] = '\0';
                    write_string_to_data(token + 1);
                } else {
                    err = STRUCT_INVALID_STRING;
                    return ERROR;
                }
            }
            else 
            {
                err = STRUCT_EXPECTED_STRING;
                return ERROR;
            }
        }
        else
        {
            err = EXPECTED_COMMA_BETWEEN_OPERANDS;
            return ERROR;
        }
    }
    else
    {
        err = STRUCT_INVALID_NUM;
        return ERROR;
    }
    if(!end_of_line(next_list_token(token, line)))
    {
        err = STRUCT_TOO_MANY_OPERANDS;
        return ERROR;
    }
    return NO_ERROR;
}

/** Analyzing  a .data directive */
int handle_data_directive(char *line)
{
    char token[20]; 

    boolean number = FALSE, comma = FALSE;

    while(!end_of_line(line))
    {
        line = next_list_token(token, line); /** Getting current token */

        if(strlen(token) > 0) 
        {
            if (!number) { 
                if (!is_number(token)) { 
                    err = DATA_EXPECTED_NUM;
                    return ERROR;
                }

                else {
                    number = TRUE; 
                    comma = FALSE; 
                    write_num_to_data(atoi(token)); 
                }
            }

            else if (*token != COMMA) 
            {
                err = DATA_EXPECTED_COMMA_AFTER_NUM;
                return ERROR;
            }

            else 
            {
                if(comma) {
                    err = DATA_COMMAS_IN_A_ROW;
                    return ERROR;
                }
                else {
                    comma = TRUE;
                    number = FALSE;
                }
            }

        }
    }
    if(comma == TRUE)
    {
        err = DATA_UNEXPECTED_COMMA;
        return ERROR;
    }
    return NO_ERROR;
}

/** Write number to data */
void write_num_to_data(int num)
{
    data[dc++] = (unsigned int) num;
}

/** Write string to data */
void write_string_to_data(char *str)
{
    while(!end_of_line(str))
    {
        data[dc++] = (unsigned int) *str; 
        str++;
    }
    data[dc++] = '\0'; 
}

/** Checks for addressing type */
int detect_method(char * operand)
{
    char *struct_field; 

    if(end_of_line(operand)) return NOT_FOUND;

    if (*operand == '#') { 
        operand++;
        if (is_number(operand))
            return METHOD_IMMEDIATE;
    }

    else if (is_register(operand))
        return METHOD_REGISTER;

    else if (is_label(operand, FALSE)) /** Checking if the label without a colon */
        return METHOD_DIRECT;

    else if (is_label(strtok(operand, "."), FALSE)) { 
        struct_field = strtok(NULL, ""); 
        if (strlen(struct_field) == 1 && (*struct_field == '1' || 
                *struct_field == '2'))
            return METHOD_STRUCT;
    }
    err = COMMAND_INVALID_METHOD;
    return NOT_FOUND;
}

/** Validate of given addressing types according to the opcode */
boolean command_accept_methods(int type, int first_method, int second_method)
{
    switch (type)
    {
        /* These opcodes only accept
         * Source: 0, 1, 2, 3
         * Destination: 1, 2, 3
         */
        case MOV:
        case ADD:
        case SUB:
            return (first_method == METHOD_IMMEDIATE ||
                   first_method == METHOD_DIRECT ||
                   first_method == METHOD_STRUCT ||
                   first_method == METHOD_REGISTER)
                &&
                   (second_method == METHOD_DIRECT ||
                    second_method == METHOD_STRUCT ||
                    second_method == METHOD_REGISTER);

        case LEA:
            return (first_method == METHOD_DIRECT ||
                    first_method == METHOD_STRUCT)
                   &&
                   (second_method == METHOD_DIRECT ||
                    second_method == METHOD_STRUCT ||
                    second_method == METHOD_REGISTER);

        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case RED:
        case JSR:
            return first_method == METHOD_DIRECT ||
                   first_method == METHOD_STRUCT ||
                   first_method == METHOD_REGISTER;

        case PRN:
        case CMP:
        case RTS:
        case STOP:
            return TRUE;
    }

    return FALSE;
}

/** Validate of given methods according to the opcode */
boolean command_accept_num_operands(int type, boolean first, boolean second)
{
    switch (type)
    {
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
            return first && second;

        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case RED:
        case PRN:
        case JSR:
            return first && !second;

        case RTS:
        case STOP:
            return !first && !second;
    }
    return FALSE;
}

/** Calculates number of additional words for a command */
int calculate_command_num_additional_words(int is_first, int is_second, int first_method, int second_method)
{
    int count = 0;
    if(is_first) count += num_words(first_method);
    if(is_second) count += num_words(second_method);

    if(is_first && is_second && first_method == METHOD_REGISTER && second_method == METHOD_REGISTER) count--;

    return count;
}

/** Build the first word of the command */
unsigned int build_first_word(int type, int is_first, int is_second, int first_method, int second_method)
{
    unsigned int word = 0;

    word = type;

    word <<= BITS_IN_METHOD; 

    if(is_first && is_second)
        word |= first_method;

    word <<= BITS_IN_METHOD; 

    if(is_first && is_second)
        word |= second_method;
    else if(is_first)
        word |= first_method;

    word = insert_are(word, ABSOLUTE); 

    return word;
}

/**Returns additional words of the addressing method requires */
int num_words(int method)
{
    if(method == METHOD_STRUCT) 
        return 2;
    return 1;
}

/** Handles an .extern directive */
int handle_extern_directive(char *line)
{
    char token[LABEL_LENGTH]; 

    copy_token(token, line); 
    if(end_of_line(token)) 
    {
        err = EXTERN_NO_LABEL;
        return ERROR;
    }
    if(!is_label(token, FALSE)) /** Check if token is a label without a colon */
    {
        err = EXTERN_INVALID_LABEL;
        return ERROR;  
    }  

    line = next_token(line);
    if(!end_of_line(line))
    {
        err = EXTERN_TOO_MANY_OPERANDS;
        return ERROR;
    }

    /** Add the label to the symbols table */
    if(add_label(&symbols_table, token, EXTERNAL_DEFAULT_ADDRESS, TRUE) == NULL)
        return ERROR;
    return is_error();
}

/** Checks if a given token is a label */
boolean is_label(char *token, int colon)
{
    boolean has_digits = FALSE; 
    int token_len = strlen(token);
    int i;

    /** Checks if token's length is not too short */
    if(token == NULL ||
            token_len < (colon ? MINIMUM_LABEL_LENGTH_WITH_COLON: MINIMUM_LABEL_LENGTH_WITHOUT_COLON))
        return FALSE;

    if(colon && token[token_len - 1] != ':') return FALSE; 

    if (token_len > LABEL_LENGTH) {
        if(colon) err = LABEL_TOO_LONG; 
        return FALSE;
    }
    if(!isalpha(*token)) { 
        if(colon) err = LABEL_INVALID_FIRST_CHAR;
        return FALSE;
    }

    if (colon) {
        token[token_len - 1] = '\0'; 
        token_len--;
    }
	
    /** Check if all characters are digits or letters */
    for(i = 1; i < token_len; i++)
    {
        if(isdigit(token[i]))
            has_digits = TRUE;
        else if(!isalpha(token[i])) {
            if(colon) err = LABEL_ONLY_ALPHANUMERIC;
            return FALSE;
        }
    }

    if(!has_digits)
    {
        if (find_command(token) != NOT_FOUND) {
            if(colon) err = LABEL_CANT_BE_COMMAND; 
            return FALSE;
        }
    }

    if(is_register(token))
    {
        if(colon) err = LABEL_CANT_BE_REGISTER;
        return FALSE;
    }

    return TRUE;
}