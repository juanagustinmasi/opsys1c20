#ifndef FUNCIONES_H_
#define FUNCIONES_H_
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
#include <time.h>
#include <sys/ipc.h>
#include <signal.h>
#include <ctype.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <ifaddrs.h>

#define LOG_FILE "./logFile.txt"
#define DIR_ASISTENCIA "./Asistencias/"

#define TAM_QUERY 250
#define MAX_QUEUE 10
#define INICIADO 1
#define NO_ENCONTRADO 0

typedef struct {
	
	char usuario[30];
	char password[30];
	
} t_usuario;

typedef struct {
	char nombre[30];
	char password[30];
	char rol[2];
	int cod_comision;
	
} t_datos;

typedef struct {
    char nombre[30];
    char asistencia[2];
} t_asistencia;

typedef struct {
	
	int operacion;
	char linea[100];
	
} t_request;

int creacionSocket(int *serverSocket, int *habilitar);
int comprobacionBD(char *path);
struct sockaddr_in configuracionSocket;
int bindListen(int *serverSocket);
void set(const char *ip, const char *puerto);

void *obtenerQuery(void *sockfd);
int iniciarSesion(FILE *arch, int socketCliente, t_usuario *query, t_datos *registro);
void operaciones(FILE *arch, int socketCliente, t_request *query, t_datos *usuarioLoggeado);
void enviarMensaje(const char *mensaje, int socketCliente, char *tamPaquete);
int obtenerComision(char *nombreArch);
void calcularPorcentaje(char *nomArchivo, float *presente, float *ausente, t_datos *usuarioLoggeado);
void mostrarYGuardarMSJ(char *msj);
void aceptarRequests(pthread_t *tid, int *socketCliente, int *serverSocket, struct sockaddr_in *ca, socklen_t *cl);

void sendMsg(const char *, char *, sem_t *, sem_t *, sem_t *);
int obtenerIP(char * ip);
void signal_handler(int sig);

void log_message(char *filename, char *message);
#endif
