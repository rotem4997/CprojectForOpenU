#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "preprocessor.h"
#include "prototypes.h"
#include "ext_vars.h"
#include "utils.h"


unsigned int data[MACHINE_RAM];
unsigned int instructions[MACHINE_RAM];
int ic;
int dc;
int err;
labelPtr symbols_table;
extPtr ext_list;
boolean entry_exists, extern_exists, was_error;

const char base32[32] = {
        '!', '@', '#', '$', '%', '^', '&', '*', '<', '>', 'a', 'b', 'c',
        'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
        'q', 'r', 's', 't', 'u', 'v'};

const char *commands[] = {
        "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne",
        "get", "prn", "jsr", "rts", "hlt"
};

const char *directives[] = {
        ".data", ".string", ".struct", ".entry", ".extern"
};

void reset_global_vars()
{
    symbols_table = NULL;
    ext_list = NULL;

    entry_exists = FALSE;
    extern_exists = FALSE;
    was_error = FALSE;
}

/** Handles all activities in the program */
int main(int argc, char *argv[]){
    int i;
    char *input_filename;
    FILE *fp;
    FILE *ap;
    
    for(i = 1; i < argc; i++)
    {
        input_filename = create_file_name(argv[i], FILE_INPUT); /** Appending .as to filename */
        fp = fopen(input_filename, "r");
        preprocessor(fp ,input_filename); /** Appending .am to filename */
        ap = fopen(input_filename, "r");
        if(ap != NULL) { 
            printf("************* Started %s assembling process *************\n\n", input_filename);

            reset_global_vars();
            first_pass(ap);

            if (!was_error) { 
                rewind(ap);
                second_pass(ap, argv[i]);
            }

            printf("\n\n************* Finished %s assembling process *************\n\n", input_filename);
        }
        else write_error(CANNOT_OPEN_FILE);
        free(input_filename);
    }

	return 0;
}