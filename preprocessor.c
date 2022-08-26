#include "preprocessor.h"
#include "assembler.h"
#include "utils.h"

/* Preprocessor for macros */
void preprocessor(FILE *codefile, char *filename)
{
    FILE *processedfile;
    char line[LINE_LENGTH];
    char *found, *name, *macro, *token;
    Macro *newMacro = NULL, *macroToApply = NULL, *head = NULL;
    int macroflag = 0, macrolines = 1, firstmacro = 1;
    processedfile = fopen(strcat(strtok(filename, SEPARATOR), EXPANDED_EXT), "w");

    /* searching file for macros and saving a processed file*/
    while ((fgets(line,LINE_LENGTH, codefile) != NULL))
    {
        if (!macroflag) /*If a macro is not found yet*/
        {
            found = strstr(line, MACROSTART); /*Serching for the Macro Start*/

            if (!found) /*If the Macro has already been stored in memory*/
            {
                /*Adding the Macro at the required location*/
                if ((macroToApply = isMacro(line, head)))
                    fputs(macroToApply->macro, processedfile);
                else
                    fputs(line, processedfile);
            }
            else
            {
                macroflag = 1;
                macro = (char *)malloc(LINE_LENGTH);
                strtok(line, " ");
                token = strtok(NULL, " "); /*Move to the macro name*/
                if (token == NULL)
                {
                    write_error(ER_EMPTY_MACRO);
                    continue;
                }
                newMacro = (Macro *)malloc(sizeof(Macro));
                name = (char *)malloc(strlen(token));
                strcpy(name, token);
                newMacro->name = name;
                if (firstmacro) /*If this is the first Macro*/
                {
                    head = newMacro;
                    firstmacro = 0;
                }
                else
                    addToMacroList(newMacro, head); /*Add Macro to list of Macros*/
            }
        }
        else
        {
            found = strstr(line, MACROEND); /*Searching for the end of a macro*/
            if (!found)/*continue searching for macro lines*/
            {
                macro = (char *)realloc(macro, LINE_LENGTH * macrolines);
                strcat(macro, line);
                macrolines++;
            }
            else/*end macro*/
            {
                macroflag = 0;
                newMacro->macro = macro;
                newMacro->next = NULL;
                macrolines = 1;
            }
        }
    }
    fclose(processedfile);
    freeMacros(head);
}

Macro *isMacro(char *str, Macro *head)
{
    while (head)
    {
        if (strcmp(str, head->name) == 0)
            return head;
        head = head->next;
    }
    return NULL;
}

void addToMacroList(Macro *newMacro, Macro *head)
{
    while (head->next != NULL)
        head = head->next;

    head->next = newMacro;
}

void freeMacros(Macro *head)
{
    if (!head)
        return;
    if (head->next)
        freeMacros(head->next);

    free(head->name);
    free(head);
}