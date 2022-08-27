#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>


#include "ext_vars.h"
#include "prototypes.h"
#include "utils.h"

void second_pass(FILE *ap, char *filename)
{
    char line[LINE_LENGTH]; /**Line number*/
    int line_num = 1; /** Line number start */

    ic = 0; /** Initializing IC */

    while(fgets(line, LINE_LENGTH, ap) != NULL) /** Read lines until EOF*/
    {
        err = NO_ERROR;
        if(!ignore(line)) /** Ignore line if it's blank or ; */
            read_line_second_pass(line); /** Analyze one line at a time */
        if(is_error()) { 
            was_error = TRUE; 
            write_error(line_num);
        }
        line_num++;
    }
    if(!was_error) /** Write output files  */
        write_output_files(filename);

    /** Free memory */
    free_labels(&symbols_table);
    free_ext(&ext_list);
}

/**  Analyze and extract data needed for the second pass */
void read_line_second_pass(char *line)
{
    int dir_type, command_type;
    char current_token[LINE_LENGTH]; 
    line = skip_spaces(line); 
    if(end_of_line(line)) return; 

    copy_token(current_token, line);
    if(is_label(current_token, COLON)) { 
        line = next_token(line);
        copy_token(current_token, line);
    }

    if((dir_type = find_directive(current_token)) != NOT_FOUND)
    {
        line = next_token(line);
        if(dir_type == ENTRY)
        {
            copy_token(current_token, line);
            make_entry(symbols_table, current_token); /** Create an entry for the symbol */
        }
    }

    else if ((command_type = find_command(current_token)) != NOT_FOUND) 
    {
        line = next_token(line);
        handle_command_second_pass(command_type, line);
    }
}

/**Create output files*/
int write_output_files(char *original)
{
    FILE *file;

    file = open_file(original, FILE_OBJECT);
    write_output_ob(file);

    if(entry_exists) {
        file = open_file(original, FILE_ENTRY);
        write_output_entry(file);
    }

    if(extern_exists)
    {
        file = open_file(original, FILE_EXTERN);
        write_output_extern(file);
    }

    return NO_ERROR;
}

/** Creates .ob file */
void write_output_ob(FILE *ap)
{
    unsigned int address = MEMORY_START;
    int i;
    char *param1 = convert_to_base_32(ic), *param2 = convert_to_base_32(dc);

    fprintf(ap, "%s\t%s\n\n", param1, param2); 
    free(param1);
    free(param2);

    for (i = 0; i < ic; address++, i++)
    {
        param1 = convert_to_base_32(address);
        param2 = convert_to_base_32(instructions[i]);

        fprintf(ap, "%s\t%s\n", param1, param2);

        free(param1);
        free(param2);
    }

    for (i = 0; i < dc; address++, i++)
    {
        param1 = convert_to_base_32(address);
        param2 = convert_to_base_32(data[i]);

        fprintf(ap, "%s\t%s\n", param1, param2);

        free(param1);
        free(param2);
    }

    fclose(ap);
}

/** Create .ent file*/
void write_output_entry(FILE *ap)
{
    char *base32_address;

    labelPtr label = symbols_table;
    /** Go through symbols table and print only symbols that have an entry */
    while(label)
    {
        if(label -> entry)
        {
            base32_address = convert_to_base_32(label -> address);
            fprintf(ap, "%s\t%s\n", label -> name, base32_address);
            free(base32_address);
        }
        label = label -> next;
    }
    fclose(ap);
}

/** Create .ext file */
void write_output_extern(FILE *ap)
{
    char *base32_address;
    extPtr node = ext_list;


    do
    {
        base32_address = convert_to_base_32(node -> address);
        fprintf(ap, "%s\t%s\n", node -> name, base32_address); /* Printing to file */
        free(base32_address);
        node = node -> next;
    } while(node != ext_list);
    fclose(ap);
}

/** Open a file*/
FILE *open_file(char *filename, int type)
{
    FILE *file;
    filename = create_file_name(filename, type); /* Creating filename with extension */

    file = fopen(filename, "w"); /* Opening file with permissions */
    free(filename); /* Allocated modified filename is no longer needed */

    if(file == NULL)
    {
        err = CANNOT_OPEN_FILE;
        return NULL;
    }
    return file;
}

/**Determine if source and destination operands exist by opcode */
void check_operands_exist(int type, boolean *is_src, boolean *is_dest)
{
    switch (type)
    {
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
            *is_src = TRUE;
            *is_dest = TRUE;
            break;

        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case RED:
        case PRN:
        case JSR:
            *is_src = FALSE;
            *is_dest = TRUE;
            break;

        case RTS:
        case STOP:
            *is_src = FALSE;
            *is_dest = FALSE;
    }
}

/**handle commands for the second pass - encoding additional words */
int handle_command_second_pass(int type, char *line)
{
    char first_op[LINE_LENGTH], second_op[LINE_LENGTH]; 
    char *src = first_op, *dest = second_op; 
    boolean is_src = FALSE, is_dest = FALSE; /** Source/destination operands existence */
    int src_method = METHOD_UNKNOWN, dest_method = METHOD_UNKNOWN; /** Their addressing methods */

    check_operands_exist(type, &is_src, &is_dest);

   
    if(is_src)
        src_method = extract_bits(instructions[ic], SRC_METHOD_START_POS, SRC_METHOD_END_POS);
    if(is_dest)
        dest_method = extract_bits(instructions[ic], DEST_METHOD_START_POS, DEST_METHOD_END_POS);

 
    if(is_src || is_dest)
    {
        line = next_list_token(first_op, line);
        if(is_src && is_dest) 
        {
            line = next_list_token(second_op, line);
            next_list_token(second_op, line);
        }
        else
        {
            dest = first_op; 
            src = NULL;
        }
    }

    ic++; 
    return encode_additional_words(src, dest, is_src, is_dest, src_method, dest_method);
}

/**Encodes the additional words of the operands to instructions memory */
int encode_additional_words(char *src, char *dest, boolean is_src, boolean is_dest, int src_method,
                                int dest_method) {
    
    if(is_src && is_dest && src_method == METHOD_REGISTER && dest_method == METHOD_REGISTER)
    {
        encode_to_instructions(build_register_word(FALSE, src) | build_register_word(TRUE, dest));
    }
    else 
    {
        if(is_src) encode_additional_word(FALSE, src_method, src);
        if(is_dest) encode_additional_word(TRUE, dest_method, dest);
    }
    return is_error();
}

/**Build the additional word for a register operand */
unsigned int build_register_word(boolean is_dest, char *reg)
{
    unsigned int word = (unsigned int) atoi(reg + 1); /* Getting the register's number */
 
    if(!is_dest)
        word <<= BITS_IN_REGISTER;
    word = insert_are(word, ABSOLUTE);
    return word;
}

/**Encodes a given label to memory */
void encode_label(char *label)
{
    unsigned int word; 

    if(is_existing_label(symbols_table, label)) { 
        word = get_label_address(symbols_table, label); /** Getting label's address */

        if(is_external_label(symbols_table, label)) { 
            add_ext(&ext_list, label, ic + MEMORY_START);
            word = insert_are(word, EXTERNAL);
        }
        else
            word = insert_are(word, RELOCATABLE); 

        encode_to_instructions(word); /** Encode word to memory */
    }
    else 
    {
        ic++;
        err = COMMAND_LABEL_DOES_NOT_EXIST;
    }
}

/**Encodes an additional word to instructions memory given the addressing method */
void encode_additional_word(boolean is_dest, int method, char *operand)
{
    unsigned int word = EMPTY_WORD; 
    char *temp;

    switch (method)
    {
        case METHOD_IMMEDIATE: 
            word = (unsigned int) atoi(operand + 1);
            word = insert_are(word, ABSOLUTE);
            encode_to_instructions(word);
            break;

        case METHOD_DIRECT:
            encode_label(operand);
            break;

        case METHOD_STRUCT: /** Before the dot there should be a label, and after it a number */
            temp = strchr(operand, '.');
            *temp = '\0';
            encode_label(operand); 
            *temp++ = '.';
            word = (unsigned int) atoi(temp);
            word = insert_are(word, ABSOLUTE);
            encode_to_instructions(word); 
        break;

        case METHOD_REGISTER:
            word = build_register_word(is_dest, operand);
            encode_to_instructions(word);
    }
}
