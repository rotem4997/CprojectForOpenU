#include "globals.h"
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
    new->base = base;
    new->offset =offset;
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



void free_externals_list() {
    external *it = external_list, *tmp;
    while (it != NULL) {
        tmp = it;
        it = it->next;
        free(tmp);
    }
    external_list = NULL;
}