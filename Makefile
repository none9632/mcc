objects = main.o error.o lex.o lib.o pars.o table_names.o launch.o

myCompiler : $(objects)
	gcc -g3 $(objects) -o myCompiler
main.o : main.c
	gcc -g3 -c main.c -o main.o
lib.o : lib.c
	gcc -g3 -c lib.c -o lib.o
error.o : error.c
	gcc -g3 -c error.c -o error.o
launch.o : launch.c
	gcc -g3 -c launch.c -o launch.o
pars.o : pars.c
	gcc -g3 -c pars.c -o pars.o
lex.o : lex.c
	gcc -g3 -c lex.c -o lex.o
table_names.o : table_names.c
	gcc -g3 -c table_names.c -o table_names.o
clean :
	rm $(objects) myCompiler