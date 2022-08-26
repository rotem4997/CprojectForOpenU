#include "globals.h"
#include "Header.h"
#include <string.h>
#include "stdbool.h"
#include <stdlib.h>
#include <stdio.h>


/**
 * get_machine_instruction_line - Get the machine instruction line object
 * @param current_token 
 * @param file_status 
 * @param should_skip 
 * @return machine_instruction_line* 
 */
machine_instruction_line *get_machine_instruction_line(char *current_token, status *file_status, bool *should_skip) {
    machine_instruction *mi = NULL;
    machine_instruction_line *mil = NULL;
    int OP_counter = 0;
    char * op;
    char * op2;
    if (!(mi = get_machine_instruction_line(current_token, file_status, should_skip))) {
        printf("undefined machine instruction", file_status,should_skip);
        return NULL;
    }

    mil = (machine_instruction_line *)safe_malloc(sizeof(machine_instruction_line));
    mil->mi = mi;
    mil->destination_operand = NULL;
    mil->source_operand = NULL;

    current_token = strtok(NULL, " ,\t\n");
    if (!current_token) {
        if (OP_counter == mi->amount_of_operands) {
            return mil;
        }
        printf("Invalid Num of Operands", file_status, should_skip);
        free(mil);
        return NULL;
    }

    OP_counter++;
    op = (char *)safe_malloc(sizeof(char ) * MAX_LINE_LENGTH);
    strcpy(op, current_token);

    current_token = strtok(NULL, " ,\t\n");
    if (!current_token) {
        if (OP_counter == mi->amount_of_operands) {
            mil->destination_operand = op;
            return mil;
        }
        printf("Invalid Num of Operands", file_status, should_skip);
        free(op);
        free(mil);
        return NULL;
    }

    OP_counter++;
    op2 = (char *)safe_malloc(sizeof(char ) * MAX_LINE_LENGTH);
    strcpy(op2, current_token);

    current_token = strtok(NULL, " ,\t\n");
    if (!current_token) {
        if (OP_counter == mi->amount_of_operands) {
            mil->source_operand = op;
            mil->destination_operand = op2;
            return mil;
        }
        printf("Invalid Num of Operands", file_status, should_skip);
        free(op);
        free(op2);
        free(mil);
        return NULL;
    }else {
        printf("Too Many Operands", file_status, should_skip);
        free(op);
        free(op2);
        free(mil);
        return NULL;
    }


}

addressing_types check_addressing_type(char *operand, char *record_symbol, unsigned int *register_num, int *immediate_num, status *file_status, bool *should_skip) {
    int result = 0;
    int reg = 0;
    if(!operand) {
        return NONE;
    }

    if (operand[0] == '#') {
        char *end;
        char *operand_cpy = operand;
        operand_cpy++;
        result = strtok(operand_cpy, &end);
        if (*end != '\0') {
            printf("Invalid Number", file_status, should_skip);
            return NONE;
        }
        if ((result < INT8_MIN) || (result > INT8_MAX)) {
            printf("Out Of Range");
            return NONE;
        }
        *immediate_num = result;
        return IMMEDIATE;
    }

    if ((reg = is_register(operand)) != -1) {
        *register_num = reg;
        return REGISTER;
    }

    if (is_record_addresing_type(operand, record_symbol, register_num, file_status, should_skip)) {
        return RELATIVE;
    } else if (*should_skip){
        return NONE;
    }

    if (is_valid_symbol_name(operand, file_status, true,should_skip)) { /* check if operand is valid symbol name and return direct addressing mode */
    return DIRECT;
    }
    return NONE;
}

bool analyse_operands_structure_of_machine_instruction(char *current_token, int *IC, status *file_status, bool *should_skip) {
    int add_to_ic = 0;
    word *first_word;
    word *second_word = NULL;
    word *immediate_word, *direct_word;
    unsigned int absolute_t = ((1 << 2) & 0xff);

    machine_instruction_line *mil;
    if(!(mil = get_machine_instruction_line(current_token,file_status,should_skip))) {
        return false;
    }
    first_word = new_word();
    first_word->ocw = (op_code_word *)safe_malloc(sizeof(op_code_word));
    first_word->ocw->op_code = (1 << mil->mi->opcode);
    first_word->address = *IC;
    add_to_ic++;

    if (mil->source_operand != NULL) {
        unsigned int register_num;
        int immediate_num;
        char index_symbol[MAX_LINE_LENGTH];
        addressing_types source_addresing_types = check_addresssing_types(mil->source_operand, index_symbol, &register_num, &immediate_num, file_status, should_skip);

        if (source_addresing_types == NONE) {
            free_word_chain(first_word);
            return false;
        } else if (source_addresing_types == REGISTER) {
            second_word->source_addressing_types= REGISTER;
            second_word->source_operand == register_num;
        } else if (source_addresing_types == RELATIVE) {
            second_word->source_addressing_types = RELATIVE;
            second_word->source_operand = register_num;
            add_to_ic = add_to_ic+2;
        } else if (source_addresing_types == IMMEDIATE) {
             second_word->source_addressing_types = IMMEDIATE;
            second_word->source_operand = 0;

            immediate_word = new_word();

            push_word(first_word, immediate_word);
            immediate_word->address = *IC + add_to_ic;
            add_to_ic++;
        } else {
            second_word->source_addressing_types = DIRECT;
            second_word->source_operand = 0;

            direct_word = new_word();
            direct_word->address = *IC + add_to_ic;

            push_word(second_word, direct_word);
            add_to_ic = add_to_ic+2;
        }
    }

    if (mil->destination_operand != NULL) {
        unsigned int register_num;
        int immediate_num;
        char index_symbol[MAX_LINE_LENGTH];
        addressing_types dest_addressing_types = analyse_addresssing_types(mil->destination_operand, index_symbol, &register_num, &immediate_num, file_status, should_skip);

        if (dest_addressing_types == NONE) {
            free_word_chain(first_word);
            return false;
        } else if (dest_addressing_types == REGISTER) {
            second_word->destination_addressing_types= REGISTER;
            second_word->destination_operand == register_num;
        } else if (dest_addressing_types == RELATIVE) {
            second_word->destination_addressing_types = RELATIVE;
            second_word->destination_operand = register_num;
            add_to_ic = add_to_ic+2;
        } else if (dest_addressing_types == IMMEDIATE) {
             second_word->destination_addressing_types = IMMEDIATE;
            second_word->destination_operand= 0;

            immediate_word = new_word();

            push_word(first_word, immediate_word);
            immediate_word->address = *IC + add_to_ic;
            add_to_ic++;
        } else {
            second_word->destination_addressing_types = DIRECT;
            second_word->destination_operand = 0;

            direct_word = new_word();
            direct_word->address = *IC + add_to_ic;

            push_word(second_word, direct_word);
            add_to_ic = add_to_ic+2;
        }
    }

    if (!add_word_to_word_list(first_word)) {
        printf("Cannot add word to the list", file_status);
    }
    (*IC) = (*IC)+add_to_ic;
    return true;
}

bool data_directive_parsing(char *current_token, status *file_status, bool *should_skip, int *DC) {
    bool expecting_comma = false; 
    bool status;
    while (current_token) {
        char *token_cpy = current_token;
        char current_num[MAX_LINE_LENGTH];
        int i = 0;
        if (!expecting_comma) {
            printf("Error");
            exit(1);
        } else {
            current_num[i] = (*token_cpy);
            i++;
        }
        add_value_to_data_list(DC, current_token);
        current_token++;
        return;
}
}

bool string_directive_parsing(char *current_token, status *file_status, bool *should_skip, int *DC) {
    while (current_token) {
        char *token_cpy;
        int token_len = strlen(current_token);
        if (!token_len <2 || ((current_token[token_len-1]) != '"')) {
            printf("Error");
            exit(1);
        } else {
            token_cpy = current_token;
            current_token[token_len-1] = '\0';
            token_cpy++;
        }
        add_value_to_data_list(DC, current_token);
        current_token++;
        return;
}
}

bool instructions_directive_parsing(char *current_token, status *file_status, bool *should_skip, int *DC) {
    bool expecting_comma = false; 
    bool status;
    while (current_token) {
        char *token_cpy = current_token;
        int i = 0;
        int line = 0;
        int token_len = strlen(current_token);
        while (line < MAX_LINE_LENGTH) {
            if (!expecting_comma) {
                printf("Error");
                exit(1);
            }
            if ((*token_cpy) == i && (*token_cpy) == ','|| (*token_cpy) == i && (*token_cpy) == ' ' && (*token_cpy) == ',')
            {
                if ((*token_cpy) == '"' && current_token[token_len-1] == '\0' && (*token_cpy) == '"') {
                    data_directive_parsing(current_token,file_status,should_skip,DC);
                    string_directive_parsing(current_token,file_status,should_skip,DC);
            
                } else if ((*token_cpy) != ' ' && (*token_cpy) != '\0' && (*token_cpy) != '\n') {
                    data_directive_parsing(current_token,file_status,should_skip,DC);
                } 
            } else {
                string_directive_parsing(current_token,file_status,should_skip,DC);
            }
        }
    }
    return true;
}

