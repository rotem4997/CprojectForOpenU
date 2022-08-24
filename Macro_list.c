#include "globals.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


macro * macro_list_head = NULL;

macro *add_macro(char *macro_id){
    macro new* = NULL;
    if((new = get_macro_by_id(macro_id))){
        fprintf("cannot add new macro %s, macro already exists\n", macro_id);
        return NULL;
    }

new = malloc(sizeof(macro));
if(new == NULL){
fprintf("Cannot allocate memory");
exit(1);
}
new->macro_id = malloc(sizeof(char)* MAX_LINE_LENGTH);
strcpy(new->macro_id, macro_id);
new->macro_content = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH * MAX_MACRO_LINE_LENGTH);
new->macro_content[0] = '\0';
new->next = NULL;
if(!macro_list_head){
    macro_list_head = new;
}
else{
    macro *ptr - macro_list_head;
    while(ptr->next){
        ptr = ptr->next;
    }
    ptr->next = new;
    }
    return new;
}


macro *get_macro_by_id(char *macro_id){
    macro *ptr = macro_list_head;
    while(ptr){
        if(!strcmp(ptr->macro_id,macro_id)){
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

void free_macros_list(){
    macro *head = macro_list_head, *tmp;
    while(head!=null){
        tmp = head;
        tmp = tmp->next;
        free(tmp->macro_id);
        free(tmp->macro_content);
        free(tmp);
    }
    macro_list_head = NULL;
}