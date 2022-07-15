static entry * entry_list_head = NULL;


entry *get_entry_by_id(char *symbol_id){
    entry * it = entry_list_head;
    while (it){
        if (!strcmp(it->symbol_id,symbol_id)){
            return it;
        }
        it = it->next;
    }
    return NULL;
}

/**
 * add new entry to entry list
 * @param symbol_id
 * @return the created entry
 */
entry *add_entry(char *symbol_id){
    entry *new = NULL;
    if((new = get_entry_by_id(symbol_id))){
        return new;
    }

    new = (entry *)safe_malloc(sizeof(entry));
    strcpy(new->symbol_id,symbol_id);
    new->checked = false;
    new->next = NULL;

    if(!entry_list_head){
        entry_list_head = new;
    } else {
        entry * it = entry_list_head;
        while (it->next){
            it = it->next;
        }
        it->next = new;
    }
    return new;
}
 /**
  * free the entry list from memory
  */
void free_entry_list() {
    entry *it = entry_list_head, *tmp;
    while (it != NULL) {
        tmp = it;
        it = it->next;
        free(tmp);
    }
    entry_list_head = NULL;
}

/**
 * the function return the next unchecked entry from list
 * @return unchecked entry
 */
entry *get_next_unchecked_entry(){
    entry * it = entry_list_head;
    while (it){
        if (!it->checked) return it;
        it = it->next;
    }
    return NULL;
}

/**
 * get the head object of the entry list
 * @return entry head object
 */
entry *get_entries_list_head() {
    entry * it = entry_list_head;
    return it;
}



