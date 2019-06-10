objects = main.o error.o lex.o vector.o pars.o table_names.o launch.o

myCompiler : $(objects)
	gcc -g3 $(objects) -o myCompiler
main.o : main.c
	gcc -g3 -c main.c -o main.o
vector.o : vector.c
	gcc -g3 -c vector.c -o vector.o
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