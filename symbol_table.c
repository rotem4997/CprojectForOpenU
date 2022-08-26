#include "globals.h"
#include "Header.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char_symbols *symbol_table_head = NULL;

char_symbols *add_symbol_to_table(char* symbol_id, int value, bool is_data, bool is_string, bool is_entry, bool is_struct, bool is_external,status *file_stauts){
 char_symbols *new = NULL;

 if(new = get_symbol(symbol_id)){
     if(is_external && new->is_external){
         return new;
     }
    printf("Cannot add symbol, symbol already exists\n", symbol_id);
    return NULL;
 }
    new = malloc(sizeof(char_symbols));
    if(new == NULL){
    printf("Could not allocate memory for symbol", symbol_id);
    exit(1);    
    }   
    strcpy(new->symbol_id, symbol_id);
    new->value = value;
    new->is_data = is_data;
    new->is_entry = is_entry;
    new->is_external = is_external;
    new->is_string = is_string;
    new->is_struct = is_struct;
    new->next = NULL;

if(!symbol_table_head){
    symbol_table_head = new;
}
else{
    char_symbols *ptr = symbol_table_head;
    while(ptr->next){
        ptr = ptr->next;
    }
    ptr->next = new;
}
return new;
 }

 char_symbols *get_symbol_by_id(char *symbol_id){
     char_symbols *ptr = symbol_table_head;
     while(ptr){
         if(!strcmp(ptr->symbol_id,symbol_id)){
             return ptr;
         }
         ptr = ptr->next;
     }
     return NULL;
 }

void add_ic_to_data_symbols(int IC){
    char_symbols *it = symbol_table_head;
    while(it != NULL){
        if(it->is_data){
            it->value = it->value +IC;
        }
        it = it->next;
    }
}

void free_symbol_table(){
    char_symbols *it = symbol_table_head, *current_symbol;
    while(it!= NULL){
    current_symbol = it;
    it = it->next;
    free(current_symbol);    
    }
    symbol_table_head = NULL;
}