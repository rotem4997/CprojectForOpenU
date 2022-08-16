/**
 * utils functions
 */
#include "headers.h"
#include "consts.h"

/**
 * log error and exist
 * @param err
 * @param file_status
 */
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

/**
 * safe malloc
 * @param n - size of object
 * @return pointer to created object
 */
void *safe_malloc(size_t n) {
    void *p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "Fatal: failed to allocate memory.\n");
        abort();
    }
    return p;
}

/**
 * check if line is valid
 * @param line - line to check
 * @param fp - the file
 * @param file_status - file status object
 * @return if the line is valid
 */
bool is_valid_line(char *line, FILE *fp, status *file_status) {
    if (strchr(line, '\n') == NULL && !feof(fp)) {
        char err_msg[MAX_LINE_LENGTH];
        sprintf(err_msg, "line maximum length is %d", MAX_LINE_LENGTH);
        log_error_wrapper(err_msg, file_status,NULL);
        return false;
    }
    return true;
}
/**
 * concat 2 strings
 * @param s1
 * @param s2
 * @return concat string
 */
char *concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


/**
 * extract the 4 bits from position p
 * @param number - number to extract bits from
 * @param p - position
 * @return - extracted bits
 */
unsigned int extract_4_bits_from_pos(long number, int p) {
    number = number >> p;
    return ((number) & 0xF);
}

/**
 * create new file status
 * @param file_name - file name
 * @param ext - file extension
 * @return new file status
 */
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

/**
 * free all linked lists in project
 */
void free_linked_lists(){
    free_symbol_table();
    free_word_list();
    free_macro_list();
    free_entry_list();
    free_data_list();
    free_externals_list();
}


machine_directive *get_machine_directive(char *current_token) {
    int i;
    for (i = 0; i < NO_OF_MACHINE_DIRECTIVES; i++) {
        if (!strcmp(current_token, machine_directives[i].name)) {
            return &machine_directives[i];
        }
    }
    return NULL;
}

/**
 * check if current token is a valid register and if so return register number
 * @param current_token - register name
 * @return register index in register array
 */
int is_register(char *current_token) {
    int i;
    for (i = 0; i < NO_OF_REGISTERS; i++) {
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
    return (get_machine_directive(current_token) || (is_register(current_token) != -1));
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
            log_error_wrapper("symbol must start with alphabetic letter", file_status,should_skip);
        }
        return false;
    }

    token_cpy = current_token;
    while ((*token_cpy) != '\0') {
        if (!isalnum(*token_cpy)) {
            if (should_log){
                log_error_wrapper("new_symbol must contain only alphanumeric characters", file_status,should_skip);
            }
            return false;
        }
        token_cpy++;
    }
    if (is_reserved_word(current_token)) {
        if (should_log) {
            log_error_wrapper("cannot use reserved word as a symbol name", file_status,should_skip);
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

/**
 * the function gets word value and address and write it to the file in address - hex format
 * @param file_to_write - file to write the line to
 * @param word_bits - the word value
 * @param address - the word address
 */

void write_num_to_file(FILE * file_to_write, unsigned long word_bits, int address){
    word_bits = ((word_bits) & 0xFFFFF);

    fprintf(file_to_write, "%04d A%hx-B%hx-C%hx-D%hx-E%hx\n" ,address,extract_4_bits_from_pos(word_bits,16),extract_4_bits_from_pos(word_bits,12),
            extract_4_bits_from_pos(word_bits,8),
            extract_4_bits_from_pos(word_bits,4),
            extract_4_bits_from_pos(word_bits,0));
}
