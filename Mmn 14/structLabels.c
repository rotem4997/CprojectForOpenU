#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "utils.h"
#include "ext_vars.h"

/**Offsets the addresses of a certain group of labels by a given delta*/
void offset_addresses(labelPtr label, int num, boolean is_data)
{
    while(label)
    {
        
        if(!(label -> external) && (is_data ^ (label -> inActionStatement)))
        {
            label -> address += num;
        }
        label = label -> next;
    }
}

/**Search a label in the list and changes his entry field to TRUE ,if the label doesn't exist return FALSE. */
int make_entry(labelPtr h, char *name)
{
    labelPtr label = get_label(h, name);
    if(label != NULL)
    {
        if(label -> external)
        {
            err = ENTRY_CANT_BE_EXTERN;
            return FALSE;
        }
        label -> entry = TRUE;
        entry_exists = TRUE; 
        return TRUE;
    }
    else
        err = ENTRY_LABEL_DOES_NOT_EXIST;
    return FALSE;
}

/** Return the address of a given label, if the label doesn't exist return FALSE*/
unsigned int get_label_address(labelPtr h, char *name)
{
    labelPtr label = get_label(h, name);
    if(label != NULL) return label -> address;
    return FALSE;
}

/**Check if a label is in the array and an external label */
boolean is_external_label(labelPtr h, char *name)
{
    labelPtr label = get_label(h, name);
    if(label != NULL) return label -> external;
    return FALSE;
}

/**Check if a given name is a name of a label in the list */
boolean is_existing_label(labelPtr h, char *name)
{
    return get_label(h, name) != NULL;
}

/**Get Label function */
labelPtr get_label(labelPtr h, char *name)
{
	while(h)
	{
        if(strcmp(h->name,name)==0) /* we found a label with the name given */
			return h;
		h=h->next;
	}
	return NULL;
}

/**Adds a new label to the list of labels */
labelPtr add_label(labelPtr *hptr, char *name, unsigned int address, boolean external, ...)
{	
	va_list p;
	
	labelPtr t=*hptr;
	labelPtr temp; /* Auxiliary variable to store the info of the label and add to the list */

	if(is_existing_label(*hptr, name))
	{
		err = LABEL_ALREADY_EXISTS;
		return NULL;
	}
	temp=(labelPtr) malloc(sizeof(Labels)); 
	if(!temp) 
	{
		printf("\nerror, cannot allocate memory\n");
		exit(ERROR);
	}

	
	strcpy(temp->name, name);
    temp -> entry = FALSE;
	temp -> address = address;
	temp -> external = external;

	if(!external) 
	{
		va_start(p,external);
		temp -> inActionStatement = va_arg(p,boolean);
	}
    else
    {
        extern_exists = TRUE;
    }

	
	if(!(*hptr))
	{	
		*hptr = temp;
		temp -> next = NULL;
		return temp;	
	}

	
	while(t -> next != NULL)
		t = t->next;
	temp -> next = NULL;
	t -> next = temp;

	va_end(p);
	return temp;
}

/**Frees the allocated memory for the symbols table*/
void free_labels(labelPtr *hptr)
{

	labelPtr temp;
	while(*hptr)
	{
		temp=*hptr;
		*hptr=(*hptr)->next;
		free(temp);
	}
}

/**Deletes label from list*/
int delete_label(labelPtr *hptr, char *name)
{
    
    labelPtr temp = *hptr;
    labelPtr prevtemp;
    while (temp) {
        if (strcmp(temp->name, name) == 0) {
            if (strcmp(temp->name, (*hptr)->name) == 0) {
                *hptr = (*hptr)->next;
                free(temp);
            } else {
                prevtemp->next = temp->next;
                free(temp);
            }
            return 1;
        }
        prevtemp = temp;
        temp = temp->next;
    }
    return 0;

}

/* This function prints the list */
void print_labels(labelPtr h)
{
    while (h)
    {
        printf("\nname: %s, address: %d, external: %d", h->name, h->address, h->external);
        if (h->external == 0)
            printf(", is in action statement: %d -> ", h->inActionStatement);
        else
            printf(" -> ");
        h = h->next;
    }
    printf("*");
}
