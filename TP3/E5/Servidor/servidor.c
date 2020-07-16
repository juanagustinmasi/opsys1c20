/*-----------------------ENCABEZADO------------------------------------------------------------------------

 Nombre del programa: servidor.c
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

#include "funciones.h"

int main(int argc, char *argv[])
{
    struct sockaddr_in listenAddress;

    if (argc != 3)
    {
        if ((argc == 1) || (argc == 2) && (strcasecmp(argv[1], "-help") == 0 || strcasecmp(argv[1], "-h") == 0 || strcasecmp(argv[1], "-?") == 0))
        {

            printf("\nEste programa es un servidor de gestión de asistencias de la UNLaM.\n");
            printf("Para ejecutarlo debe ingresar como parámetros el nro. de puerto y\n");
            printf("el archivo donde se encuentra la información de los usuarios.\n\n");
            printf("Luego podrá ejecutar uno o más procesos cliente para interactuar con este servidor.\n\n");
            printf("Ejemplo de ejecución: \"./Servidor 8081 usuario.txt\"\n\n");
            printf("Señal para terminar proceso: SIGTERM.\n\n");

            return 0;
        }

        printf("\nParámetros incorrectos, consulte la ayuda con: \"-help\", \"-h\" o \"-?\".\n");
        return ERROR_PARAMETROS;
    }

    char logServidorConectado[500];
    int socketCliente, serverSocket;
    char ip[50];
    struct sockaddr_in addr;
    socklen_t addrlen;
    pthread_t thread;
    int v = 1;

	//estas senales las ignora
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    
    signal(SIGTERM, terminarProceso);
    signal(SIGINT, terminarProceso);

    // Validación del archivo de usuarios
    if (existeArchivoUsuarios(argv[2]) == 1)
        return ERROR_ARCHIVO_USUARIOS;

    // Obtención de IP
    if (ipConfig(ip))
    {
        printf("Error de conexión.\nSaliendo...\n\n");
        return ERROR_IP;
    }

    // Creación del socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Error de conexión.\nSaliendo...\n\n");
        return ERROR_SOCKET;
    }
    
    // Configuración del socket
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(int)) == -1)
    {
        printf("Error de conexión 8.\nSaliendo...\n\n");
        return ERROR_SOCKET;
    }

    bzero(&(listenAddress.sin_zero), 8);
    listenAddress.sin_family = AF_INET;
    listenAddress.sin_addr.s_addr = inet_addr(ip);
    listenAddress.sin_port = htons(atoi(argv[1]));

    if (bind(serverSocket, (struct sockaddr *)&listenAddress, sizeof(struct sockaddr_in)) < 0)
    {
        printf("Puerto %d en uso, intente con otro puerto.\nSaliendo...", ntohs(listenAddress.sin_port));
        return ERROR_SOCKET;
    }

    listen(serverSocket, 10);
    sprintf(logServidorConectado, "\nServidor CONECTADO correctamente.\nIP: %s\nPuerto: %d\n", ip, ntohs(listenAddress.sin_port));
    printf("%s\n", logServidorConectado);
    grabarLog("./servidor.log", logServidorConectado);
    fflush(stdout);

    addrlen = sizeof(struct sockaddr_in);

    while (TRUE)
        recibeConexion(&thread, &socketCliente, &serverSocket, &addr, &addrlen);

    return 0;
}

/*
# ------------------------------------FIN -----------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------# 
*/