#include "globals.h"
#include "Header.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static data *data_list_head = NULL;

void print_data_to_file(FILE * file_to_write, int IC) {
    data * it = data_list_head;
    while (it){
        unsigned long word_bits = 0;
        word_bits = (( (1 <<2) & 0xFF)<<16) | ((unsigned short int)(it->value) & 0xFFFFF) ;
        write_num_to_file(file_to_write,word_bits,it->dc+IC);
        it = it->next;
    }
}

data *add_value_to_data_list(int dc, int value){

    data *new = NULL;
    new = malloc(sizeof(data));
    if(new == NULL){
        fprintf(stderr, "could not allocate memory ");
        exit(1);
    }
    new->dc = dc;
    new->next =NULL;
    if(!data_list_head){
        data_list_head = new;
    }
    else{
        data *ptr = data_list_head;
        while(ptr->next){
            ptr = ptr->next;
        }
        ptr->next = new;
    }
    return new;
}

void free_data_list(){
    data *it = data_list_head, *tmp;

    while(it!= NULL){
        tmp = it;
        it = it->next;
        free(tmp);
    }
    data_list_head = NULL;
}

