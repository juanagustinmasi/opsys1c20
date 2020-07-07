/*-----------------------ENCABEZADO------------------------------------------------------------------------

 Nombre del programa: funciones.h
 Trabajo practico: 3
 Ejercicio: 5
 Entrega: 3ra
 Integrantes:
	    Daiana Gomez Nespola, DNI 38005120
	    Juan Masi, DNI 37981647
        Guido Delbo, DNI 36982261
	    Nicolas Fortunato, DNI 37864810
	    Damian Perez, DNI 35375255

 ----------------------FIN ENCABEZADO---------------------------------------------------------------------*/


#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <ifaddrs.h>

#define TRUE 1
#define FALSE 0
#define FIN "FIN"
#define ERROR "ERROR"
#define ERROR_ARCHIVO_USUARIOS 11
#define ERROR_IP 12
#define ERROR_SOCKET 13
#define ERROR_PARAMETROS 14

typedef struct {
	char nombre[50];
	char clave[50];
	char rol[2];
	int cod_comision;
} tInfo;

typedef struct {
    char nombre[50];
    char asistencia[2];
} tAsistencia;

typedef struct {
	char usuario[50];
	char clave[50];
} tUsuario;

typedef struct {
	int codigo;
	char dato[100];
} tInfoSocket;

char archivoUsuarios[500];

void grabarLog(char *nombreArchivo, char *msg);
int loguea(FILE *arch, int socketCliente, tUsuario *info, tInfo *registro);
int existeArchivoUsuarios(char *path);
void recibeConexion(pthread_t *thread, int *socketCliente, int *serverSocket, struct sockaddr_in *addr, socklen_t *addrlen);
void *recibeConsulta(void *sockfd);
void procesaConsulta(FILE *arch, int socketCliente, tInfoSocket *info, tInfo *usuarioActual);
void enviaAlSocket(const char *mensaje, int socketCliente, char *tamMsg);
int parseaComision(char *nombreArchivo);
void calcularPorcentaje(char *nombreArchivo, float *presente, float *ausente, tInfo *usuarioActual);
void imprimirMsg(char *msg);
int ipConfig(char * ip);
void terminarProceso();

#endif

/*
# ------------------------------------FIN -----------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------# 
*/