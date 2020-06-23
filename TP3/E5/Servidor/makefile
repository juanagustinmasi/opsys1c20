all : Servidor

Servidor : servidor.c funcionesServidor.o funcionesAuxiliares.o
	gcc -o Servidor servidor.c funcionesServidor.o funcionesAuxiliares.o -pthread -lrt

funcionesServidor.o: funciones.h funcionesServidor.c
	gcc -c funcionesServidor.c -pthread -lrt

funcionesAuxiliares.o: funciones.h funcionesAuxiliares.c
	gcc -c funcionesAuxiliares.c -pthread -lrt

clean :
	rm *.o
