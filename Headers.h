typedef struct macro{
    char *macro_id;
    char *macro_content;
    struct macro *next;
}macro;

typedef struct line_info{
    char *file_name;
    int *line_num;
    char *content;
}line_info;




#define END_OF_MACRO "endmacro"
#define START_OF_MACRO "macro"
#define ERROR_IN_LINE "error in %s: %s\n"
#define MAX_MACRO_LINE_LENGTH 6
#define MAX_LINE_LENGTH 100
#define MAX_SYMBOL_LENGTH 31