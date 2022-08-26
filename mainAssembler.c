#include "globals.h"
#include "Header.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    /**char ASCII_symbol[ASCII_MAX_LENGTH_VALUE]={
      '!'
      ,'@'
      ,'#'
      ,'$'
      ,'%'
      ,'^'
      ,'&'
      ,'*'
      ,'<'
      ,'>'
      ,'a'
      ,'b'
      ,'c'
      ,'d'
      ,'e'
      ,'f'
      ,'g'
      ,'h'
      ,'i'
      ,'j'
      ,'k'
      ,'l'
      ,'m'
      ,'n'
      ,'o'
      ,'p'
      ,'q'
      ,'r'
      ,'s'
      ,'t'
      ,'u'
      ,'v'
    };*/
    int i;
    for (i = 1; i < argc; ++i) { /* iterate through the arguments*/
        FILE *file_to_read = NULL; /* file pointer */
        status *file_status = NULL; /* file status */

        int IC = IC_INIT_VALUE; /* initialize the ic and dc for each file*/
        int DC = 0;

        if ((file_to_read = read_file_with_extension(argv[i], ".as", "r"))) { /* check if .as file exist and if so pass the assembly file to the pre assembler */
            file_status = new_status(argv[i], ".as"); /*init new file status object*/
            pre_assembler(file_to_read, argv[i],file_status); /* send file to pre assembler */
            if (file_status->errors_flag) { /* if error found during the macro parsing, free lists close file and continue !*/
                fclose(file_to_read);
                free_status(file_status);
                free_linked_lists();
                continue;
            }
            fclose(file_to_read); /* close file */
            free_status(file_status); /* free status object */
        } else {
            continue; /* if file not exist, continue */
        }

        if ((file_to_read = read_file_with_extension(argv[i], ".am", "r"))) { /* check if .am file exist and if so pass the file to the first pass */
            file_status = new_status(argv[i], ".am"); /*init new file status object*/
            first_pass(file_to_read, &IC, &DC, file_status); /* send file to first pass */
            if (file_status->errors_flag) { /* if error found in first pass, free lists close file and continue! */
                fclose(file_to_read);
                free_status(file_status);
                free_linked_lists();
                continue;
            }
            free_status(file_status); /* reset file status */
            file_status = new_status(argv[i], ".am");
            second_pass(file_status);  /* send file to second pass */
            if (file_status->errors_flag) { /* if errors found in second pass, close file, free lists and continue */
                fclose(file_to_read);
                free_status(file_status);
                free_linked_lists();
                continue;
            }
            free_status(file_status);
            fclose(file_to_read);
        } else {
            continue;
        }
        create_ob_file(argv[i],IC,DC); /* create object file */
        create_ext_file(argv[i]); /* create externals file */
        create_ent_file(argv[i]); /* create entries file */



        free_linked_lists(); /* free lists */
    }
    return 0;
}
