#include "globals.h"
#include "utils.h"
#include "string.h"
#include "stdbool.h"
#define MOVE_TO_NOT_WHITE(string, index) \
        for (;string[(index)] && (string[(index)] == '\t' || string[(index)] == ' '); (++(index)))\
        ;



bool is_valid_label_name(char *name) {
	/* Check length, first char is alpha and all the others are alphanumeric, and not saved word */
	return name[0] && strlen(name) <= 31 && isalpha(name[0]) && is_alphanumeric_str(name + 1);
	       
}



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
        if ((!token_len <2 || ((current_token[token_len-1]) != '"')) {
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




