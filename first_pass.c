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
        addressing_types source_addresing_types = analyse_addresssing_types(mil->source_operand, index_symbol, &register_num, &immediate_num, file_status, should_skip);

        if (source_addresing_types == NONE) {
            free_word_chain(first_word);
            return false;
        } else if (source_addresing_types == REGISTER) {
            second_word->source_addressing_types= REGISTER;
            second_word->source_registery == register_num;
        } else if (source_addresing_types == RECORD) {
            second_word->source_addressing_types = RECORD;
            second_word->source_addressing_registery = register_num;
            add_to_ic = add_to_ic+2;
        } else if (source_addresing_types == IMMEDIATE) {
             second_word->source_addressing_types = IMMEDIATE;
            second_word->source_registery = 0;

            immediate_word = new_word();
            immediate_word->dlw = new_data_word(&absolute_t,&immediate_num, NULL);

            push_word(first_word, immediate_word);
            immediate_word->address = *IC + add_to_ic;
            add_to_ic++;
        } else {
            second_word->source_addressing_types = DIRECT;
            second_word->source_registery = 0;

            direct_word = new_word();
            direct_word->address = *IC + add_to_ic;

            direct_word->dlw = new_data_word(NULL, NULL, mil->source_operand);
            push_word(second_word, direct_word);
            add_to_ic = add_to_ic+2;
        }
    }

    if (mil->destination_operand != NULL) {
        unsigned int register_num;
        int immediate_num;
        char index_symbol[MAX_LINE_LENGTH];
        addressing_types dest_addresing_types = analyse_addresssing_types(mil->destination_operand,, index_symbol, &register_num, &immediate_num, file_status, should_skip);

        if (dest_addresing_types == NONE) {
            free_word_chain(first_word);
            return false;
        } else if (dest_addresing_types == REGISTER) {
            second_word->destination_addresing_types= REGISTER;
            second_word->destination_registery == register_num;
        } else if (dest_addresing_types == RECORD) {
            second_word->destination_addresing_types = RECORD;
            second_word->destination_registery = register_num;
            add_to_ic = add_to_ic+2;
        } else if (dest_addresing_types == IMMEDIATE) {
             second_word->destination_addressing_types = IMMEDIATE;
            second_word->destination_registery = 0;

            immediate_word = new_word();
            immediate_word->dlw = new_data_word(&absolute_t,&immediate_num, NULL);

            push_word(first_word, immediate_word);
            immediate_word->address = *IC + add_to_ic;
            add_to_ic++;
        } else {
            second_word->destination_addressing_types = DIRECT;
            second_word->destination_registery = 0;

            direct_word = new_word();
            direct_word->address = *IC + add_to_ic;

            direct_word->dlw = new_data_word(NULL, NULL, mil->destination_operand);
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


bool is_directive(char *current_token, status *file_status, bool *should_skip, char *new_symbol, int *DC) {
    if (current_token[0] != '.') { /* if current word doesn't start with ., it's not a directive*/
        return false;
    }
    if (!strcmp(current_token, DATA_CODE_DIRECTIVE)) { /* check if data directive and if so, validate and parse*/
        if (new_symbol) {
            if (!add_symbol_to_symbol_table(new_symbol, *DC, false, false, false, true, file_status)){
                *should_skip = true;
                return false;
            }
        }
        current_token = strtok(NULL, " \t\n");
        if (!current_token) {
            log_error_wrapper("data directive cannot be empty!", file_status,should_skip);
            return false;
        }
        return data_directive_parsing(current_token, file_status, should_skip, DC);
    } else if (!strcmp(current_token, STRING_CODE_DIRECTIVE)) { /* check if string directive and if so, validate and parse*/
        if (new_symbol) {
            if (!add_symbol_to_symbol_table(new_symbol, *DC, false, false, false, true, file_status)){
                *should_skip = true;
                return false;
            }
        }
        current_token = strtok(NULL, "\n");
        if (!current_token) {
            log_error_wrapper("sting directive cannot be empty!", file_status,should_skip);
            return false;
        }
        return string_directive_parsing(current_token, file_status, should_skip, DC);
    } else if (!strcmp(current_token, EXTERN_CODE_DIRECTIVE)) { /* check if data directive and if so, validate and parse */
        current_token = strtok(NULL, "\n");
        if (!current_token) {
            log_error_wrapper("extern directive cannot be empty!", file_status,should_skip);
            return false;
        }
        if (!is_valid_symbol_name(current_token,file_status,true,should_skip)) {
            return false;
        }
        if (!add_symbol_to_symbol_table(current_token, 0, true, false, false, false, file_status)){
            *should_skip = true;
            return false;
        }
        return true;
    } else if (!strcmp(current_token, ENTRY_CODE_DIRECTIVE)) { /* check if entry directive and if so, add to entry list */
        /* instead of reading the file lines again and look for entries in the second pass
         * the entries saved to entries linked list and then, in the second pass the code iterate through this list
         * instead of reading the file again.
         * */
        current_token = strtok(NULL, "\n");
        if (!current_token) {
            log_error_wrapper("entry directive cannot be empty!", file_status,should_skip);
            return false;
        }
        if (!is_valid_symbol_name(current_token,file_status,true,should_skip)) { /* validate entry symbol name*/
            return false;
        }
        add_entry(current_token);
        return true;
    } else {
        log_error_wrapper("this code directive is not supported", file_status,should_skip); /* invalid directive*/
        return false;
    }
}



void first_pass(FILE *file_to_read,int *IC, int *DC, status *file_status) {
    char line[MAX_LINE_LENGTH]; /* line string */
    char *current_token = NULL; /* current word of the line */
    char *new_symbol = NULL; /* symbol pointer */
    bool should_skip = false; /* should skip current line flag */
    char line_copy[MAX_LINE_LENGTH]; /* declaration of line copy */

    while (fgets(line, MAX_LINE_LENGTH, file_to_read)) { /* read new line from file */
        file_status->line_number++; /* increase line number in file status */
        should_skip = false; /* reset should skip line status */

        strcpy(line_copy, line);
        if (!is_valid_line(line_copy, file_to_read, file_status)) { /* check if line is valid*/
            continue;
        }
        new_symbol = NULL;
        current_token = strtok(line_copy, " \t\n");
        if (!current_token || !strcmp(current_token, ";")) { /* if comment or empty line, skip current line */
            continue;
        }

        if (is_new_symbol(current_token, file_status, &should_skip, new_symbol)) { /* look for new symbol, and if exist set current symbol pointer to  parsed symbol*/
            new_symbol = current_token;
            current_token = strtok(NULL, " \t\n"); /* move to next word*/
            if (!current_token) { /* if there is no word after symbol, return error */
                log_error_wrapper("symbol must be assigned with value", file_status,NULL);
                continue;
            }
        } else if (should_skip) { /*if should skip current line due to error, continue */
            continue;
        }

        if (is_directive(current_token, file_status, &should_skip, new_symbol, DC) || should_skip) { /* check if its a directive line and if so and the directive parsed correctly or error found, continue */
            continue;
        }

        if (new_symbol) { /* if it's not a directive, its must be machine directive, so first check if there is symbol and if so save it to symbol list*/
            if (!add_symbol_to_symbol_table(new_symbol, *IC, false, true, false, false, file_status)){
                continue;
            }
        }

        if (!analyse_operands_structure_of_machine_directive(current_token,IC,file_status,&should_skip)){ /* parse machine directive line and continue if there is error, continue */
            continue;
        }
    }
    if (file_status->errors_flag) { /* if errors found in the first pass, log that error found and skip file */
        fprintf(stderr,"error found in first pass, skipping file!\n" );
        return;
    }
    add_ic_to_data_symbols(*IC); /* update data symbol addresses with final ic */
    if((*IC)+(*DC) > MEMORY_MAX_SIZE) { /* check if code memory reached limit */
        log_error_wrapper("out of memory!" , file_status,&should_skip);
    }
    fprintf(stdout, "first pass of %s finished successfully\n", file_status->file_name);
}
