#include "globals.h"
#include "utils.h"
#include "string.h"
#include "stdbool.h"

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

