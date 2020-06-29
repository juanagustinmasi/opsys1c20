all: Consumidor PoductorPagos ProductorAsistencias
Consumidor: consumidor.c librerias.h
	gcc -o consumidor.o consumidor.c -lpthread

PoductorPagos: productorPagos.c librerias.h
	gcc -o productorPagos.o productorPagos.c -lpthread

ProductorAsistencias: productorAsistencias.c librerias.h
	gcc -o productorAsistencias.o productorAsistencias.c -lpthread

.PHONY: clean
clean:
	rm -rf *.o