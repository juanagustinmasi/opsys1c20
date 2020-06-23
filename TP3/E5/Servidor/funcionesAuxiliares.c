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

int obtenerIP(char *ip)
{
    struct ifaddrs *iflist, *iface;

    if (getifaddrs(&iflist) < 0)
    {
        perror("getifaddrs");
        return 1;
    }

    for (iface = iflist; iface; iface = iface->ifa_next)
    {
        int af = iface->ifa_addr->sa_family;
        const void *addr;
        char addrp[INET6_ADDRSTRLEN];

        switch (af)
        {
        case AF_INET:
            addr = &((struct sockaddr_in *)iface->ifa_addr)->sin_addr;
            break;
        case AF_INET6:
            addr = &((struct sockaddr_in6 *)iface->ifa_addr)->sin6_addr;
            break;
        default:
            addr = NULL;
        }

        if (addr)
        {
            if (inet_ntop(af, addr, addrp, sizeof addrp) == NULL)
            {
                perror("inet_ntop");
                continue;
            }
            if (!strcmp(iface->ifa_name, "lo") == 0)
            {
                printf("La direccion del servidor es %s\n", addrp);
                strcpy(ip, addrp);
                return 0;
            }
        }
    }

    freeifaddrs(iflist);
    return 1;
}

void signal_handler(int sig)
{
    switch (sig)
    {
    case SIGHUP:
        log_message(LOG_FILE, "Proceso finalizado.");
        exit(0);
        break;
    case SIGTERM:
        log_message(LOG_FILE, "Proceso finalizado.");
        exit(0);
        break;
    }
}
void log_message(char *filename, char *message)
{
    FILE *logfile;
    logfile = fopen(filename, "a");
    if (!logfile)
        return;
    fprintf(logfile, "%s\n", message);
    fclose(logfile);
}
