/*-----------------------ENCABEZADO------------------------------------------------------------------------

 Nombre del programa: cliente.c
 Trabajo practico: 3
 Ejercicio: 5
 Entrega: 4ta
 Integrantes:
	    Daiana Gomez Nespola, DNI 38005120
	    Juan Masi, DNI 37981647
        Guido Delbo, DNI 36982261
	    Nicolas Fortunato, DNI 37864810
	    Damian Perez, DNI 35375255

 ----------------------FIN ENCABEZADO---------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define FIN "FIN"
#define ERROR "ERROR"
#define ERROR_PARAMETROS 11
#define ERROR_SOCKET 12

typedef struct
{
	char usuario[50];
	char clave[50];
} tUsuario;

typedef struct
{
	int codigo;
	char dato[100];
} tInfoSocket;

struct sockaddr_in listenAddress;
char *token, c, rol[10], respuesta[200];
char delim[] = "=\n";
int socketCliente, tamanioRespuesta, comision;
tInfoSocket info;
tUsuario usuario;

void terminarProceso()
{
	printf("\nSaliendo...\n");
	info.codigo = 3;
    send(socketCliente, &info, sizeof(tInfoSocket), 0);
	close(socketCliente);
	exit(0);
}

void terminarProcesoPrevLogin()
{
	printf("\nSaliendo...\n");
	sprintf(usuario.usuario, "C");
    send(socketCliente, &usuario, sizeof(tUsuario), 0);
	close(socketCliente);
	exit(0);
}

void mostrarMenu(char *rol, int cod)
{
	if (strcmp(rol, "DOCENTE") == 0)
	{
		if (info.codigo == 1)
		{
			printf("\n\n*~*~*~*~*~*~*~*~*~*~*~*~*~*~ Listado de Asistencias *~*~*~*~*~*~*~*~*~*~*~*~*~*~\n\n\n\n");
			fgetc(stdin);
			printf("\nIngresar fecha de asistencias (AAAA-MM-DD): ");
			scanf("%[^\n]", info.dato);
			fgetc(stdin);
			send(socketCliente, &info, sizeof(tInfoSocket), 0);
			memset(respuesta, 0, strlen(respuesta));
			recv(socketCliente, respuesta, sizeof(int), 0);
			tamanioRespuesta = atoi(respuesta);
			recv(socketCliente, respuesta, tamanioRespuesta, 0);

			if (strstr(respuesta, ERROR) == NULL)
			{
				printf("Listado de asistencias:\n");
				while (strcmp(respuesta, FIN) != 0)
				{
					printf("%s", respuesta);
					memset(respuesta, 0, strlen(respuesta));
					recv(socketCliente, respuesta, sizeof(int), 0);
					tamanioRespuesta = atoi(respuesta);
					recv(socketCliente, respuesta, tamanioRespuesta, 0);
				}
			}
			else
			{
				printf("\nNo se encontraron asistencias para la fecha ingresada.");
			}

			printf("\nPresione enter para volver.\n");
			fgetc(stdin);
			scanf("%c", &c);
			system("clear");
		} else
		{
			printf("*~*~*~*~*~*~*~*~*~*~*~*~*~*~* Carga de Asistencias *~*~*~*~*~*~*~*~*~*~*~*~*~*~*\n\n\n\n");

			send(socketCliente, &info, sizeof(tInfoSocket), 0);
			memset(respuesta, 0, strlen(respuesta));
			recv(socketCliente, respuesta, sizeof(int), 0);
			tamanioRespuesta = atoi(respuesta);
			recv(socketCliente, respuesta, tamanioRespuesta, 0);

			if (strstr(respuesta, ERROR) == NULL)
			{
				while (strcmp(respuesta, FIN) != 0)
				{
					printf("Alumno: %s \n", respuesta);
					printf("Asistencia (P/A): ");
					fgetc(stdin);
					scanf("%[^\n]", info.dato);

					send(socketCliente, &info, sizeof(tInfoSocket), 0);
					memset(respuesta, 0, strlen(respuesta));
					recv(socketCliente, respuesta, sizeof(int), 0);
					tamanioRespuesta = atoi(respuesta);
					recv(socketCliente, respuesta, tamanioRespuesta, 0);
				}
			}
			else
			{
				printf("\nEl archivo de asistencias del día de la fecha ya fue cargado.\n");
			}

			printf("\nPresione enter para volver.\n");
			fgetc(stdin);
			scanf("%c", &c);
			system("clear");
		}
	}
	else
	{
		if (strcmp(rol, "ALUMNO") == 0)
		{
			if (info.codigo == 1)
			{
				printf("*~*~*~*~*~*~*~*~*~*~*~*~*~*~ Consulta de Asistencia *~*~*~*~*~*~*~*~*~*~*~*~*~*~\n\n\n\n");
				fgetc(stdin);
				printf("\nIngresar fecha de asistencia (AAAA-MM-DD): ");
				scanf("%[^\n]", info.dato);
				fgetc(stdin);
				memset(respuesta, 0, strlen(respuesta));
				send(socketCliente, &info, sizeof(tInfoSocket), 0);
				recv(socketCliente, respuesta, sizeof(int), 0);
				tamanioRespuesta = atoi(respuesta);
				recv(socketCliente, respuesta, tamanioRespuesta, 0);
				printf("%s\n\n", respuesta);

				printf("Presione enter para volver.\n");
				fgetc(stdin);
				scanf("%c", &c);
				system("clear");
			} else
			{
				printf("*~*~*~*~*~*~*~*~*~*~*~*~*~ Porcentaje de Asistencias *~*~*~*~*~*~*~*~*~*~*~*~*~*\n\n\n\n");
				send(socketCliente, &info, sizeof(tInfoSocket), 0);
				memset(respuesta, 0, strlen(respuesta));
				recv(socketCliente, respuesta, sizeof(int), 0);
				tamanioRespuesta = atoi(respuesta);
				recv(socketCliente, respuesta, tamanioRespuesta, 0);
				printf("%s\n\n", respuesta);

				printf("Presione enter para volver.\n");
				fgetc(stdin);
				scanf("%c", &c);
				system("clear");
			}
		}
	}
}

int main(int argc, char *argv[])
{

	if (argc != 3)
	{
		if ((argc == 1) || (argc == 2) && (strcasecmp(argv[1], "-help") == 0 || strcasecmp(argv[1], "-h") == 0 || strcasecmp(argv[1], "-?") == 0))
		{
			printf("\nEste programa es un cliente para el servidor de gestión de asistencias de la UNLaM.\n");
			printf("Una vez que el servidor se encuentra ejecutando y entrega su IP y puerto,\n");
			printf("se debe ejecutar este cliente con dichos datos como parámetros.\n\n");
			printf("Luego de loguearse podrá realizar distintas consultas a la base de datos\n");
			printf("del sistema dependiendo de su rol en la universidad (Alumno o Docente).\n\n");
			printf("Ejemplo de ejecución: \"./Cliente 192.168.0.9 8081\"\n\n");
			printf("Señal para terminar proceso: SIGTERM.\n\n");

			return 0;
		}

		printf("\nParámetros incorrectos, consulte la ayuda con: \"-help\", \"-h\" o \"-?\".\n");
        return ERROR_PARAMETROS;
	}

		//estas senales las ignora
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
	
	signal(SIGINT, terminarProcesoPrevLogin);
	signal(SIGTERM, terminarProcesoPrevLogin);

	if ((socketCliente = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("Error de conexión.\nSaliendo...\n\n");
		return ERROR_SOCKET;
	}

	bzero(&(listenAddress.sin_zero), 8);
	listenAddress.sin_family = AF_INET;
	listenAddress.sin_addr.s_addr = inet_addr(argv[1]);
	listenAddress.sin_port = htons(atoi(argv[2]));

	if (connect(socketCliente, (struct sockaddr *)&listenAddress, sizeof(listenAddress)) == -1)
	{
		printf("Error de conexión.\nSaliendo...\n\n");
		return ERROR_SOCKET;
	}

	printf("Usuario: ");
	scanf("%[^\n]", usuario.usuario);
	fgetc(stdin);
	printf("Contraseña: ");
	scanf("%[^\n]", usuario.clave);
	fgetc(stdin);

	send(socketCliente, &usuario, sizeof(tUsuario), 0);
	recv(socketCliente, respuesta, sizeof(int), 0);
	tamanioRespuesta = atoi(respuesta);
	recv(socketCliente, respuesta, tamanioRespuesta, 0);

	while (strcmp(respuesta, ERROR) == 0)
	{
		printf("Error en nombre de usuario o contraseña, vuelva a intentarlo.\n\n");
		printf("Usuario: ");
		scanf("%[^\n]", usuario.usuario);
		fgetc(stdin);
		printf("\nContraseña: ");
		scanf("%[^\n]", usuario.clave);
		fgetc(stdin);

		memset(respuesta, 0, sizeof(respuesta));
		send(socketCliente, &usuario, sizeof(tUsuario), 0);
		recv(socketCliente, respuesta, sizeof(int), 0);
		tamanioRespuesta = atoi(respuesta);
		recv(socketCliente, respuesta, tamanioRespuesta, 0);
	}

	signal(SIGINT, terminarProceso);
	signal(SIGTERM, terminarProceso);

	
	token = strtok(respuesta, delim);

	if (token != NULL)
		strcpy(rol, token);
	else {
		printf("\nError en la base de datos.\n");
		terminarProceso();
	}

	token = strtok(NULL, delim);

	if (token != NULL)
	{
		comision = atoi(token);
		token = NULL;
		system("clear");
	}
	else {
		printf("\nError en la base de datos.\n");
		terminarProceso();
	}
	
	printf("\n------------------------------\n%s: %s\n------------------------------\n\n", rol, usuario.usuario);
	
	do
	{
		printf("*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~ Menú Principal *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~\n\n\n\n");

		if (strcmp(rol, "DOCENTE") == 0)
		{
			printf("1. Consultar asistencia de alumnos\n");
			printf("2. Cargar asistencias de alumnos\n");
			printf("3. Salir\n\n\n");
		}
		else
		{
			printf("1. Consultar asistencia en determinada fecha\n");
			printf("2. Calcular porcentaje de asistencias\n");
			printf("3. Salir\n\n\n");
		}

		printf("*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~\n\n\n\n");

		printf("Opción: ");
		scanf("%d", &info.codigo);
		system("clear");

		if (info.codigo < 1 || info.codigo > 3)
		{
			system("clear");
			printf("Opción incorrecta.\n\n\n\n");
		}
		else if (info.codigo < 3)
		{
			mostrarMenu(rol, comision);
		}
	} while (info.codigo != 3);

	send(socketCliente, &info, sizeof(tInfoSocket), 0);
	printf("\n\n\nSaliendo...\n");
	close(socketCliente);
	
	return 0;
}

/*
# ------------------------------------FIN -----------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------# 
*/