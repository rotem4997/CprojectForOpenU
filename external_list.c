#include "globals.h"
#include "Header.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static external *external_list = NULL;


external *add_to_external_list(char *symbol_id , int base, int offset){
    external *new = NULL;
    new = malloc(sizeof(external));
    if (new == NULL){
        fprintf(stderr,"could not allocate memory ");
        exit(1);
    }
    strcpy(new->symbol_name, symbol_id);
    new->next = NULL;
    if(!external_list){
        external_list = new;
    } else {
        external * ptr = external_list;
        while (ptr->next){
            ptr = ptr->next;
        }
        ptr->next = new;
    }
    return new;
}


external *get_head_externals() {
    external * it = external_list;
    return it;
}


void print_externals_to_file(FILE * file_to_write) {
    external * it = external_list;
    while (it){ /* for each external */
        printf(file_to_write,"%d, %d\n\n", it->symbol_name, it->symbol_name);
        it = it->next;
    }
}

void free_externals_list() {
    external *it = external_list, *tmp;
    while (it != NULL) {
        tmp = it;
        it = it->next;
        free(tmp);
    }
    external_list = NULL;
}