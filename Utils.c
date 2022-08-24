#include "globals.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "stdbool.h"
#include "string.h"
#include "utils.h"


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
			return true; /* No valid symbol, and no try to define one */
		}
		return false;
	}
	symbol_dest[0] = '\0';
	return false; /* There was no error */
}

bool is_alphanumeric_str(char *string) {
	int i;
	/*check for every char in string if it is non alphanumeric char if it is function returns true*/
	for (i = 0; string[i]; i++) {
		if (!isalpha(string[i]) && !isdigit(string[i])) return false;
	}
	return true;
}

bool is_reserved_word(char *name) {
	int fun, opc;
	/* check if register or command */
	get_opcode_func(name, &opc, (funct *) &fun);
	if (opc != NONE_OP || get_register_by_name(name) != NONE_REG || find_instruction_by_name(name) != NONE_INST) return TRUE;

	return false;
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
addressing_types analyse_addressing_types(char *operand, char *index_symbol, unsigned int *register_num, int *immediate_num , status *file_status, bool *should_skip){
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

        if ((result < INT8_MIN) || (result > INT8_MAX)) { /* if immediate number is out of range (16 signed int) return error */
            log_error_wrapper("out of range", file_status,should_skip);
            return NONE;
        }
        *immediate_num = result; /* set immediate number with the parsed value*/
        return IMMEDIATE;
    }


    if ((reg = is_register(operand)) != -1) { /* check if operand is registry and if so, set registry number*/
        *register_num = reg;
        return DIRECT;
    }

    // if(is_index_addressing_mode(operand, index_symbol,register_num, file_status, should_skip)) { /* check if operand is index and if so, set index_symbol and register_num */
    //     return INDEX;
    // } else if (*should_skip){
    //     return NONE;
    // }

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


bool process_string_instruction(line_info line, int index, long *data_img, long *dc) {
	char temp_str[MAX_LINE_LENGTH];
	char *last_quote_location = strrchr(line.content, '"');
	MOVE_TO_NOT_WHITE(line.content, index)
	if (line.content[index] != '"') {
		/* something like: LABEL: .string  hello, world\n - the string isn't surrounded with "" */
		printf_line_error(line, "Missing opening quote of string");
		return false;
	} else if (&line.content[index] == last_quote_location) { /* last quote is same as first quote */
		printf_line_error(line, "Missing closing quote of string");
		return false;
	} else {
		int i;
		/* Copy the string including quotes & everything until end of line */
		for (i = 0;line.content[index] && line.content[index] != '\n' &&
		       line.content[index] != EOF; index++,i++) {
				temp_str[i] = line.content[index];
		}
		/* Put string terminator instead of last quote: */
		temp_str[last_quote_location - line.content] = '\0';
		for(i = 1;temp_str[i] && temp_str[i] != '"'; i++) {
			/* sort of strcpy but with dc increment */
			data_img[*dc] = temp_str[i];
			(*dc)++;
		}
		/* Put string terminator */
		data_img[*dc] = '\0';
		(*dc)++;
	}
	/* Return processed chars count */
	return true;
}

bool process_data_instruction(line_info line, int index, long *data_img, long *dc) {
	char temp[80], *temp_ptr;
	long value;
	int i;
	MOVE_TO_NOT_WHITE(line.content, index)
	if (line.content[index] == ',') {
		printf_line_error(line, "Unexpected comma after .data instruction");
	}
	do {
		for (i = 0;
		     line.content[index] && line.content[index] != EOF && line.content[index] != '\t' &&
		     line.content[index] != ' ' && line.content[index] != ',' &&
		     line.content[index] != '\n'; index++, i++) {
			temp[i] = line.content[index];
		}
		temp[i] = '\0'; /* End of string */
		if (!is_int(temp)) {
			printf_line_error(line, "Expected integer for .data instruction (got '%s')", temp);
			return false;
		}
		/* Now let's write to data buffer */
		value = strtol(temp, &temp_ptr, 10);

		data_img[*dc] = value;

		(*dc)++; /* a word was written right now */
		MOVE_TO_NOT_WHITE(line.content, index)
		if (line.content[index] == ',') index++;
		else if (!line.content[index] || line.content[index] == '\n' || line.content[index] == EOF)
			break; /* End of line/file/string => nothing to process anymore */
		/* Got comma. Skip white chars and check if end of line (if so, there's extraneous comma!) */
		MOVE_TO_NOT_WHITE(line.content, index)
		if (line.content[index] == ',') {
			printf_line_error(line, "Multiple consecutive commas.");
			return false;
		} else if (line.content[index] == EOF || line.content[index] == '\n' || !line.content[index]) {
			printf_line_error(line, "Missing data after comma");
			return false;
		}
	} while (line.content[index] != '\n' && line.content[index] != EOF);
	return true;
}