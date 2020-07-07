all: Consumidor PoductorPagos ProductorAsistencias
Consumidor: consumidor.c librerias.h
	gcc -o consumidor.o consumidor.c -lpthread -lrt

PoductorPagos: productorPagos.c librerias.h
	gcc -o productorPagos.o productorPagos.c -lpthread -lrt

ProductorAsistencias: productorAsistencias.c librerias.h
	gcc -o productorAsistencias.o productorAsistencias.c -lpthread -lrt

.PHONY: clean
clean:
	rm -rf *.o