objects = main.o gen.o error.o launch.o pars.o scan.o table.o

myCompiler : $(objects)
	gcc $(objects) -o myCompiler
main.o : main.c
	gcc -c main.c -o main.o 
gen.o : gen.c 
	gcc -c gen.c -o gen.o 
error.o : error.c 
	gcc -c error.c -o error.o 
launch.o : launch.c 
	gcc -c launch.c -o launch.o 
pars.o : pars.c 
	gcc -c pars.c -o pars.o 
scan.o : scan.c 
	gcc -c scan.c -o scan.o 
table.o : table.c  
	gcc -c table.c -o table.o 
clean :
	rm $(objects)