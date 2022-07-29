#include "globals.h"
#include "utils.h"
#include "string.h"
#include "stdbool.h"


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
    if (!(mi = get_machine_instruction(current_token))) {
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
        result = strtol(operand_cpy, &end, 10);
        if (*end != '\0') {
            printf("Invalid Number", file_status, should_skip);
            return NONE;
        }
        IF ((result < INT8_MIN) || (result > INT8_MAX)) {
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
        return RECORD;
    } else if (*should_skip){
        return NONE;
    }

    if (is_valid_label_name(operand, file_status, true,should_skip)) { /* check if operand is valid symbol name and return direct addressing mode */
    return DIRECT;
    }
    return NONE;
}

}

void data_directive_parsing(char *current_token, status *file_status, bool *should_skip, int *DC) {
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

void string_directive_parsing(char *current_token, status *file_status, bool *should_skip, int *DC) {
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

