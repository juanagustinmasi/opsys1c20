#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define TAM_QUERY 250

typedef struct {
	
	char usuario[30];
	char password[30];
	
} t_usuario;

typedef struct {
	
	int operacion;
	char linea[100];
	
} t_request;

void getHelp();
struct sockaddr_in configuracionSocket;

#endif // FUNCIONES_H_INCLUDED
