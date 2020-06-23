all : Cliente

Cliente : cliente.c funciones.o
	gcc -o Cliente cliente.c funciones.o -lpthread -lrt

funciones.o: funciones.h funciones.c
	gcc -c funciones.c -lpthread -lrt
	
clean :
	rm *.o
