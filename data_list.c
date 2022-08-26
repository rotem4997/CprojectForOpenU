#include "globals.h"
#include "Header.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static data *data_list_head = NULL;
 char error_msg[MAX_LINE_LENGTH]; 

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

