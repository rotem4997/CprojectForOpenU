all: assembler

assembler: data_list.o entry_list.o file_handling.o first_pass.o macro_list.o pre_assembly.o Second_pass.o symbol_table.o word_list.o utils.o external_list.o mainAssembler.o
	gcc -g -ansi -Wall -pedantic data_list.o entry_list.o file_handling.o first_pass.o macro_list.o pre_assembly.o Second_pass.o symbol_table.o word_list.o utils.o external_list.o mainAssembler.o -lm -o assembler


data_list.o: data_list.c
	gcc -c -ansi -Wall -pedantic data_list.c -o data_list.o

entry_list.o: entry_list.c
	gcc -c -ansi -Wall -pedantic entry_list.c -o entry_list.o

file_handling.o: file_handling.c
	gcc -c -ansi -Wall -pedantic  file_handling.c -o file_handling.o

first_pass.o: first_pass.c
	gcc -c -ansi -Wall -pedantic first_pass.c -o first_pass.o

macro_list.o: macro_list.c
	gcc -c -ansi -Wall -pedantic macro_list.c -o macro_list.o

pre_assembly.o: pre_assembly.c
	gcc -c -ansi -Wall -pedantic pre_assembly.c -o pre_assembly.o

Second_pass.o: Second_pass.c
	gcc -c -ansi -Wall -pedantic Second_pass.c -o Second_pass.o

symbol_table.o: symbol_table.c
	gcc -c -ansi -Wall -pedantic symbol_table.c -o symbol_table.o

word_list.o: word_list.c
	gcc -c -ansi -Wall -pedantic word_list.c -o word_list.o

utils.o: utils.c
	gcc -c -ansi -Wall -pedantic utils.c -o utils.o

external_list.o: external_list.c
	gcc -c -ansi -Wall -pedantic external_list.c -o external_list.o

mainAssembler.o: mainAssembler.c
	gcc -c -ansi -Wall -pedantic mainAssembler.c -o mainAssembler.o