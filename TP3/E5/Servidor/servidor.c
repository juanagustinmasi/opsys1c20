#include "funciones.h"
/*
***********************************************************************************
 Nombre Del Script:        Servidor
 Trabajo Practico Nro.:    3
 Ejercicio Nro.:           5
 Entrega Nro.:             1
 Integrantes:
    Apellido            Nombre                  DNI
    --------------------------------------------------
   Fernández		    Jonathan		      37226233
   Suriano				Lucas				  40429973
   Cicarone				Florencia 			  40712842
   Muñoz				Rocio Celeste		  39788890
   Sosa					Melisa Agustina 	  40464205
***********************************************************************************
*/
char ip[16];
int main(int argc, char *argv[])
{

    socklen_t cl = sizeof(struct sockaddr_in);
    struct sockaddr_in ca;
    int socketCliente, serverSocket, sockfd, habilitar = 1;
    pthread_t tid;

    if (argc < 2)
    {
        printf("Por favor, ingrese un puerto y el archivo de USUARIO\n");
        printf("Ejemplos:\n");
        printf("\t\t./Servidor 9999 usuario.txt\n");
        printf("\t\t./Servidor 54222 ../usuario.txt\n");

        exit(255);
    }
    if (comprobacionBD(argv[2]) == 1)
        return 1;

    if (obtenerIP(ip))
    {
        printf("\nOcurrio un error al determina la IP local.\n");
        exit(1);
    }

    if (creacionSocket(&serverSocket, &habilitar) == 1)
        return 1;

    ///Le paso IP y PUERTO
    set(ip, argv[1]);

    bindListen(&serverSocket);

	//estas senales las ignora
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    //aca handlea
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    ///Lo dejo corriendo como demonio
    while (1)
        aceptarRequests(&tid, &socketCliente, &serverSocket, &ca, &cl);

    return 0;
}
