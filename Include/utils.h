#ifndef _UTILS_H
#define _UTILS_H

#include "globals.h"

bool find_label(line_info line, char *symbol_dest); /** Finds the defined label in the code if exists */


bool is_valid_label_name(char* name); /**  Returns whether a label can be defined with the specified name */

#endif