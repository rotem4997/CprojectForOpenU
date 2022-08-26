#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include <string.h>
#include <stdlib.h>
#include "global.h"
#define MACROSTART "macro"
#define MACROEND "endmacro"


/*Definition Macro Struct*/
typedef struct Macro
{
    char *name;
    char *macro;
    struct Macro *next;
} Macro;

/**
 * @brief The function will process the code files and expand the macros in it.
 * The result will be save to a new file with .am extension.
 * @param codefile Pointer to the original file
 * @param filename the filename
 */
void preprocessor(FILE *codefile, char *filename);

/**
 * @brief The function checks if there's a macro in the line, if it does conatain a macro it will be expanded.
 * @param str The line
 * @param head The start of the
 * @return Pointer to the desired macro, NULL if it does not exist.
 */
Macro *isMacro(char *str, Macro *head);

/**
 * @brief The function will add a macro to the macro list.
 * @param newMacro the new macro to be added
 * @param head the beginning of the list.
 */
void addToMacroList(Macro *newMacro, Macro *head);

/**
 * @brief The function will free the macro list.
 * @param head the beginning of the list.
 */
void freeMacros(Macro *head);
#endif