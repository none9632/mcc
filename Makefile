objects = main.o gen.o error.o launch.o pars.o scan.o table.o

myCompiler : $(objects)
	gcc -g3 $(objects) -o myCompiler 
main.o : main.c
	gcc -g3 -c main.c -o main.o 
gen.o : gen.c 
	gcc -g3 -c gen.c -o gen.o 
error.o : error.c 
	gcc -g3 -c error.c -o error.o 
launch.o : launch.c 
	gcc -g3 -c launch.c -o launch.o 
pars.o : pars.c 
	gcc -g3 -c pars.c -o pars.o 
scan.o : scan.c 
	gcc -g3 -c scan.c -o scan.o 
table.o : table.c  
	gcc -g3 -c table.c -o table.o 
clean :
	rm $(objects)