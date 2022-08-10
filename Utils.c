///NEED TO DEFINE OPCODE FUNCTION check

bool is_valid_label_name(char *name) {
	/* Check length, first char is alpha and all the others are alphanumeric, and not saved word */
	return name[0] && strlen(name) <= 31 && isalpha(name[0]) && is_alphanumeric_str(name + 1) &&
	       !is_reserved_word(name);
}

#define MOVE_TO_NOT_WHITE(string, index) \
        for (;string[(index)] && (string[(index)] == '\t' || string[(index)] == ' '); (++(index)))\
        ;

bool find_label(line_info line, char *symbol_dest) {//this function checks if the label we have found is valid//
	int j, i;
	i = j = 0;

	/* Skip white chars at the beginning anyway */
	MOVE_TO_NOT_WHITE(line.content, i)

	/* Let's allocate some memory to the string needed to be returned */
	for (; line.content[i] && line.content[i] != ':' && line.content[i] != EOF && i <= MAX_LINE_LENGTH; i++, j++) {
		symbol_dest[j] = line.content[i];
	}
	symbol_dest[j] = '\0'; /* End of string */

	/* if it was a try to define label, print errors if needed. */
	if (line.content[i] == ':') {
		if (!is_valid_label_name(symbol_dest)) {
			 fprintf(stderr, "Label name is invalid! must contain up to 30 charecters and can be only AlphaNumeric");
			symbol_dest[0] = '\0';
			return TRUE; /* No valid symbol, and no try to define one */
		}
		return FALSE;
	}

	
	symbol_dest[0] = '\0';
	return FALSE; /* There was no error */
}

bool is_alphanumeric_str(char *string) {
	int i;
	/*check for every char in string if it is non alphanumeric char if it is function returns true*/
	for (i = 0; string[i]; i++) {
		if (!isalpha(string[i]) && !isdigit(string[i])) return FALSE;
	}
	return TRUE;
}

bool is_reserved_word(char *name) {
	int fun, opc;
	/* check if register or command */
	get_opcode_func(name, &opc, (funct *) &fun);
	if (opc != NONE_OP || get_register_by_name(name) != NONE_REG || find_instruction_by_name(name) != NONE_INST) return TRUE;

	return FALSE;
}

reg get_register_by_name(char *name) {
	if (name[0] == 'r' && isdigit(name[1]) && name[2] == '\0') {
		int digit = name[1] - '0'; /* convert digit ascii char to actual single digit */
		if (digit >= 0 && digit <= 7) return digit;
	}
	return NONE_REG; /* No match */
}


instruction find_instruction_by_name(char *name) {
	struct instruction_lookup_item *curr_item;
	for (curr_item = instructions_lookup_table; curr_item->name != NULL; curr_item++) {
		if (strcmp(curr_item->name, name) == 0) {
			return curr_item->value;
		}
	}
	return NONE_INST;
}
/////////Adresing method - need to build the LOG_ERROR_WRAPPER/////
addressing_mode analyse_addressing_mode(char *operand, char *index_symbol, unsigned int *register_num, int *immediate_num , status *file_status, bool *should_skip){
    int result = 0; /* immediate number initialize */
    int reg = 0; /* registry number initialize */
    if(!operand){ /* if the operand is empty return null */
        return NONE;
    }

    if (operand[0] == '#'){ /* if the operand starts with #, check for immediate addressing mode */
        char *end;
        char *operand_cpy = operand;
        operand_cpy++;
        result = strtol(operand_cpy, &end, 10); /* get immediate number and look for valid number*/
        if (*end != '\0') {
            log_error_wrapper("invalid number", file_status,should_skip);
            return NONE;
        }

        if ((result < INT16_MIN) || (result > INT16_MAX)) { /* if immediate number is out of range (16 signed int) return error */
            log_error_wrapper("out of range", file_status,should_skip);
            return NONE;
        }
        *immediate_num = result; /* set immediate number with the parsed value*/
        return IMMEDIATE;
    }


    if ((reg = is_register(operand)) != -1) { /* check if operand is registry and if so, set registry number*/
        *register_num = reg;
        return REGISTER_DIRECT;
    }

    if(is_index_addressing_mode(operand, index_symbol,register_num, file_status, should_skip)) { /* check if operand is index and if so, set index_symbol and register_num */
        return INDEX;
    } else if (*should_skip){
        return NONE;
    }

    if (is_valid_symbol_name(operand, file_status, true,should_skip)) { /* check if operand is valid symbol name and return direct addressing mode */
        return DIRECT;
    }
    return NONE;
}
void log_error_wrapper(char *err, status *file_status,bool *should_skip) {
    char error_msg[MAX_LOG_SIZE];
    if (should_skip != NULL) {
        *should_skip = true;
    }
    file_status->errors_flag = true;
    sprintf(error_msg, ERROR_WRAPPER, file_status->file_name, file_status->line_number, err);
    fprintf(stderr,"%s" ,error_msg);
}

void get_opcode_func(char *cmd, PC_Commands *opcode_out, PC_Commands *funct_out) {
	struct cmd_lookup_element *e;
	*opcode_out = NONE_OP;
	*funct_out = NONE_FUNCT;
	/* iterate through the lookup table, if commands are same return the opcode of found. */
	for (e = lookup_table; e->cmd != NULL; e++) {
		if (strcmp(e->cmd, cmd) == 0) {
			*opcode_out = e->op;
			*funct_out = e->fun;
			return;
		}
	}
}

bool is_valid_line(char *line, FILE *fp, status *file_status) {
    if (strchr(line, '\n') == NULL && !feof(fp)) {
        char err_msg[MAX_LINE_LENGTH];
        sprintf(err_msg, "line maximum length is %d", MAX_LINE_LENGTH);
        log_error_wrapper(err_msg, file_status,NULL);
        return false;
    }
    return true;
