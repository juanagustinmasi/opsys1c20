all: Servidor

Servidor: servidor.c funciones.o
	gcc -o Servidor servidor.c funciones.o -pthread -lrt

funciones.o: funciones.h funciones.c
	gcc -c funciones.c -pthread -lrt

clean:
	rm *.o