#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#define MACROSTART "macro"
#define MACROEND "endmacro"


/*Definition Macro Struct*/
typedef struct Macro
{
    char *name;
    char *macro;
    struct Macro *next;
} Macro;

/**Process the code files and expand the macros in it.*/
void preprocessor(FILE *codefile, char *filename);

/**Checks if there's a macro in the line, if it does conatain a macro it will be expanded.*/
Macro *isMacro(char *str, Macro *head);

/**Adds a macro to the macro list.*/
void addToMacroList(Macro *newMacro, Macro *head);

/**Free the macro list*/
void freeMacros(Macro *head);
#endif
