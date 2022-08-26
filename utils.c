#include "globals.h"
#include "Header.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void log_panic_wrapper(char *err, status *file_status) {
    char error_msg[MAX_LOG_SIZE];
    sprintf(error_msg, ERROR_WRAPPER, file_status->file_name, file_status->line_number, err);
    fprintf(stderr, "%s", error_msg);
    exit(1);
}

/**
 * log error and update file status. set should skip if exist
 * @param err
 * @param file_status
 * @param should_skip
 */
void log_error_wrapper(char *err, status *file_status,bool *should_skip) {
    char error_msg[MAX_LOG_SIZE];
    if (should_skip != NULL) {
        *should_skip = true;
    }
    file_status->errors_flag = true;
    sprintf(error_msg, ERROR_WRAPPER, file_status->file_name, file_status->line_number, err);
    fprintf(stderr,"%s" ,error_msg);
}


/**
 * log error without line and update file status. set should skip if exist
 * @param err
 * @param file_status
 * @param should_skip
 */
void log_error_without_line_wrapper(char *err, status *file_status,bool *should_skip) {
    char error_msg[MAX_LOG_SIZE];
    if (should_skip != NULL) {
        *should_skip = true;
    }
    file_status->errors_flag = true;
    sprintf(error_msg, ERROR_WRAPPER_NO_LINE, file_status->file_name, err);
    fprintf(stderr,"%s" ,error_msg);
}

void free_linked_lists(){
    free_symbol_table();
    free_word_list();
    free_macro_list();
    free_entry_list();
    free_data_list();
    free_externals_list();
}

bool is_valid_label_name(char *name) {
	/* Check length, first char is alpha and all the others are alphanumeric, and not saved word */
	return name[0] && strlen(name) <= 31 && isalpha(name[0]) && is_alphanumeric_str(name + 1) &&
	       !is_reserved_word(name);
}

machine_instruction *get_machine_instruction(char *current_token) {
    int i;
    for (i = 0; i < NUM_OF_OPCODE ; i++) {
        if (!strcmp(current_token, machine_instructions[i].name)) {
            return &machine_instructions[i];
        }
    }
    return NULL;
}

#define MOVE_TO_NOT_WHITE(string, index) \
        for (;string[(index)] && (string[(index)] == '\t' || string[(index)] == ' '); (++(index)))\
        ;

bool find_label(line_info line, char *symbol_dest) {
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
			printf(stderr, "Label name is invalid! must contain up to 30 charecters and can be only AlphaNumeric");
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


struct instruction_lookup_item {
	char *name;
	instruction value;
};

static struct instruction_lookup_item
		instructions_lookup_table[] = {
		{"string", STRING},
		{"data",   DATA},
		{"entry",  ENTRY},
		{"extern", EXTERNAL},
		{NULL, NONE_INST}
};

instruction find_instruction_by_name(char *name) {
	struct instruction_lookup_item *curr_item;
	for (curr_item = instructions_lookup_table; curr_item->name != NULL; curr_item++) {
		if (strcmp(curr_item->name, name) == 0) {
			return curr_item->value;
		}
	}
	return NONE_INST;
}

void *safe_malloc(size_t n) {
	void *p = malloc(n);
	if (p == NULL) {
		printf("FATAL: failed to aloocate memory.\n");
		exit(1);
	}
	return p;
}

bool is_valid_line(char *line, FILE *fp, status *file_status) {
    if (strchr(line, '\n') == NULL && !feof(fp)) {
        char err_msg[MAX_LINE_LENGTH];
        sprintf(err_msg, "line maximum length is %d", MAX_LINE_LENGTH);
        printf("Error 1", file_status);
        return false;
    }
    return true;
}

bool process_string_instruction(line_info line, int index, long *data_img, long *dc) {
	char temp_str[MAX_LINE_LENGTH];
	char *last_quote_location = strrchr(line.content, '"');
	MOVE_TO_NOT_WHITE(line.content, index)
	if (line.content[index] != '"') {
		/* something like: LABEL: .string  hello, world\n - the string isn't surrounded with "" */
		printf("Missing opening quote of string", line);
		return false;
	} else if (&line.content[index] == last_quote_location) { /* last quote is same as first quote */
		printf("Missing closing quote of string", line);
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
		printf("Unexpected comma after .data instruction", line);
	}
	do {
		for (i = 0;
		     line.content[index] && line.content[index] != EOF && line.content[index] != '\t' &&
		     line.content[index] != ' ' && line.content[index] != ',' &&
		     line.content[index] != '\n'; index++, i++) {
			temp[i] = line.content[index];
		}
		temp[i] = '\0'; /* End of string */
		if (!isdigit(temp)) {
			printf("Expected integer for .data instruction (got '%s')", line, temp);
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
			printf("Multiple consecutive commas.", line);
			return false;
		} else if (line.content[index] == EOF || line.content[index] == '\n' || !line.content[index]) {
			printf("Missing data after comma", line);
			return false;
		}
	} while (line.content[index] != '\n' && line.content[index] != EOF);
	return true;
}

int is_register(char *current_token) {
    int i;
    for (i = 0; i < NUM_OF_REGISTERS; i++) {
        if (!strcmp(current_token, registers[i])) {
            return i;
        }
    }
    return -1;
}
/**
 * check if current token is register or op code
 * @param current_token - current word
 * @return if is reserverd word
 */
bool is_reserved_word(char *current_token) {
    return (get_machine_instruction (current_token) || (is_register(current_token) != -1));
}

/**
 * check if current word is a valid symbol name
 * @param current_token - current word
 * @param file_status - file status object
 * @param should_log - should log error flag
 * @param should_skip - should skip line flag
 * @return if its a valid symbol name
 */
bool is_valid_symbol_name(char *current_token, status *file_status,bool should_log, bool *should_skip){
    char *token_cpy;
    if (!isalpha(*current_token)) {
        if (should_log){
            printf("symbol must start with alphabetic letter", file_status,should_skip);
        }
        return false;
    }

    token_cpy = current_token;
    while ((*token_cpy) != '\0') {
        if (!isalnum(*token_cpy)) {
            if (should_log){
            	printf("new_symbol must contain only alphanumeric characters", file_status,should_skip);
            }
            return false;
        }
        token_cpy++;
    }
    if (is_reserved_word(current_token)) {
        if (should_log) {
            printf("cannot use reserved word as a symbol name", file_status,should_skip);
        }
        return false;
    }
    return true;
}

/**
 * check if the current word is a valid new symbol declaration
 * @param current_token
 * @param file_status
 * @param should_skip
 * @param new_symbol
 * @return if its a valid new symbol name
 */
bool is_new_symbol(char *current_token, status *file_status, bool *should_skip, char *new_symbol) {
    int token_len;
    token_len = strlen(current_token);
    if (current_token[token_len - 1] != ':') {
        new_symbol = NULL;
        return false;
    }
    current_token[token_len - 1] = '\0';
    if (!is_valid_symbol_name(current_token,file_status,true,should_skip)){
        return false;
    }
    new_symbol = malloc(strlen(current_token));
    strcpy(new_symbol, current_token);
    return true;
}

unsigned int extract_4_bits_from_pos(long number, int p) {
    number = number >> p;
    return ((number) & 0xF);
}

void write_num_to_file(FILE * file_to_write, unsigned long word_bits, int address){
    word_bits = ((word_bits) & 0xFFFFF);

    fprintf(file_to_write, "%04d A%hx-B%hx-C%hx-D%hx-E%hx\n" ,address,extract_4_bits_from_pos(word_bits,32),extract_4_bits_from_pos(word_bits,28),
            extract_4_bits_from_pos(word_bits,24),
            extract_4_bits_from_pos(word_bits,20),
			extract_4_bits_from_pos(word_bits,16),
			extract_4_bits_from_pos(word_bits,12),
			extract_4_bits_from_pos(word_bits,8),
			extract_4_bits_from_pos(word_bits,4),
            extract_4_bits_from_pos(word_bits,0));
}

status *new_status(char *file_name, char *ext) {
    status *file_status = NULL;
    file_status = (status *) safe_malloc(sizeof(status));
    file_status->file_name = concat(file_name, ext);
    file_status->line_number = 0;
    file_status->errors_flag = false;
    return file_status;
}

/**
 * free file status from memory
 * @param current_status
 */
void free_status(status *current_status) {
    free(current_status->file_name);
    free(current_status);
}

char *concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

