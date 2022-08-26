#include "globals.h"
#include "Header.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


void pre_assembler(FILE *file_to_read, char *file_name, status *file_status){
    FILE *file_to_write;
    char line[MAX_LINE_LENGTH];
    char line_copy[MAX_LINE_LENGTH];
    char* curr_word = NULL;
    macro *curr_macro = NULL;
    int macro_flag = 0;

    file_to_write = read_file_with_extension(file_name, ".as", "w");
    if(!file_to_write){
        printf("Could not create %s.as file", file_name);
        exit(1);
    }
    while(fgets(line,MAX_LINE_LENGTH, file_to_read)){
    file_status->line_number++;
    strcpy(line,line_copy);
    curr_word = strtok(line_copy, "\t\n");
    if(macro_flag){
        if(strcmp(curr_word,END_OF_MACRO)){
            strcat(curr_macro->macro_content,line);
            continue;
        }else{
            macro_flag = 0;
            curr_macro = NULL;
            continue;
        }
    }
    }
    if(!strcmp(curr_word,START_OF_MACRO)){
        curr_word = strtok(NULL, " \t\n");
        if(!curr_word){
        printf("Macro does not have a name, file will be skipped", file_status = NULL);   
    } 
    else if(!(curr_macro = add_macro(curr_word))){ 
        printf("cannot add the new macro, file will be skipped", file_status = NULL); 
    return; 
    }
    else { 
        macro_flag = 1;    
    }

    }else{
        macro *look_for_macro = NULL;
        if(!(look_for_macro = get_macro_by_id(curr_word))){
            fputs(line,file_to_write);
    }   else{
            fputs(look_for_macro->macro_content, file_to_write);
    }
    }
    fclose(file_to_write);
    free_macro_list();

}
