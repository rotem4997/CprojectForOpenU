/*
	This file contains definitions of types and structures that are being used in the program.
*/

#ifndef STRUCTS_H

#define STRUCTS_H

#include "assembler.h"

extern unsigned int data[]; /** Data array of words */
extern unsigned int instructions[]; /** Instructions array of words */

typedef enum {FALSE, TRUE} boolean; 

/** Defining linked list of labels */
typedef struct structLabels * labelPtr;
typedef struct structLabels {
	char name[LABEL_LENGTH]; 
	unsigned int address; 
	boolean external; 
	boolean inActionStatement; 
	boolean entry; 
	labelPtr next; 
} Labels;


typedef struct ext * extPtr;
typedef struct ext {
    char name[LABEL_LENGTH]; 
    unsigned int address; 
    extPtr next; 
    extPtr prev; 
} ext;

#endif
