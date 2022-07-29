/**
 * files functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"



char * get_full_file_name(char  *file_name, char *ext){
    char *full_file_name = (char *) safe_malloc(strlen(file_name) + strlen(ext)); 
    strcpy(full_file_name, file_name);
    strcat(full_file_name, ext); 
    return full_file_name;
}


FILE * read_file_with_extension(char *file_name, char *ext, char *modes) {
    FILE * file_to_read = NULL;
    char *full_file_name = get_full_file_name(file_name, ext);
    if (!(file_to_read = fopen(full_file_name, modes) )) {
        fprintf(stderr, "couldn't read file %s, please make sure the file exist, skipping.\n", full_file_name);
        free(full_file_name);
        return 0;
    }
    free(full_file_name);
    return file_to_read;
}


void create_ob_file(char *file_name,int IC, int DC) {
    FILE * file_to_write = NULL;
    fprintf(stdout,"creating %s.ob\n", file_name);
    if (!(file_to_write = read_file_with_extension(file_name,".ob", "w"))){
        return;
    }
    fprintf(file_to_write, "%d %d\n",IC- MEMORY_START_ADDRESS , DC);
    print_words_to_file(file_to_write);
    print_data_to_file(file_to_write,IC);
    fclose(file_to_write);
}



void create_ext_file(char *file_name) {
    FILE * file_to_write = NULL;
    external *ext = get_head_externals();
    if (ext) {
        fprintf(stdout,"creating %s.ext\n", file_name);
        if (!(file_to_write = read_file_with_extension(file_name,".ext", "w"))){
            return;
        }
        print_externals_to_file(file_to_write);
        fclose(file_to_write);
    }
}


void create_ent_file(char *file_name) {
    FILE * file_to_write = NULL;
    entry *ent = get_entries_list_head();
    if(ent){
        fprintf(stdout,"creating %s.ent\n", file_name);
        if (!(file_to_write = read_file_with_extension(file_name,".ent", "w"))){
            return;
        }
        print_entries_to_file(file_to_write);
        fclose(file_to_write);
    }
}