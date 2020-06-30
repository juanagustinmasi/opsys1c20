//  -----------------------ENCABEZADO------------------------------------------------------------------------

//  Trabajo practico: 3
//  Ejercicio: 3
//  Entrega: 2da
//  Integrantes:
// 	    Daiana Gomez Nespola, DNI 38005120
// 	    Juan Masi, DNI 37981647
//      Guido Delbo, DNI 36982261
// 	    Nicolas Fortunato, DNI 37864810
// 	    Damian Perez, DNI 35375255
//  ----------------------FIN ENCABEZADO---------------------------------------------------------------------


#include "librerias.h"

int main(int argc, char *argv[])
{

    // ayuda y validaciones
    if (argc > 1)
    {
        if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-help") == 0)
        {
            printf("Este programa, ejecuta el proceso Productor de Pagos, que se alimenta del archivo Pagos.txt\n");
            printf("No recibe ningun parámetro.\n");
            printf("Ejemplo de ejecución: \"./productorPagos.o\"\n");
            exit(1);
        }
        else
        {
            printf("La cantidad de parámetros ingresados es incorrecta.\n");
            printf("Consulte la ayuda con `-?` o `-help`\n");
            exit(2);
        }

    }
    sem_t *M = sem_open("/M", O_CREAT, 0666, 0); // mutex para la memoria compartida
    sem_t *semPagos = sem_open("/semPagos", O_CREAT, 0666, 0); // semáforo productor pagos
    sem_t *confirmaProdPagos = sem_open("/confirmaProdPagos", O_CREAT, 0666, 0); //confirmacion del productor de Pagos
    sem_t *hayMemoriaParaLeer = sem_open("/hayMemoriaParaLeer", O_CREAT, 0666, 0); 

    key_t key = ftok(ARCHIVO_SOCIOS, 1);
    if (key == -1) {
        printf ("Error al obtener la Key \n");
        exit(ERROR_KEY); 
    }  

    int shmid = shmget(key, sizeof(t_asistencia_pago), IPC_CREAT | 0666); // crea la memoria compartida

    t_asistencia_pago *pMemComp = (t_asistencia_pago *)shmat(shmid, NULL, 0); // asigna la memoria compartida a la estructura

    FILE *archBD=fopen(ARCHIVO_PAGOS, "r");
    if( archBD == NULL)
    {
        printf("Error al acceder al archivo de pagos\n");
        exit(ERROR_ARCHIVO);
    }
    
    //Utiliza la Memoria compartida
    while( !feof(archBD) )
    {
        sem_wait(semPagos);
        sem_wait(M);
        fflush(stdin);
        fscanf(archBD,"%ld;%04d-%02d-%02d\n",&(*pMemComp).dni,&(*pMemComp).fechapago.anio,&(*pMemComp).fechapago.mes,&(*pMemComp).fechapago.dia);

        strcpy(pMemComp->diaAsistencia, "");
        strcpy(pMemComp->observaciones, "");

        sem_post(M);
        sem_post(hayMemoriaParaLeer);
    }

    fclose(archBD);

    sem_wait(semPagos);
    sem_wait(M);
    strcpy(pMemComp->observaciones, FIN_ARCHIVO_PAGOS);
    sem_post(M);
    sem_post(hayMemoriaParaLeer);

    sem_post(confirmaProdPagos);

    shmdt(&shmid);
    sem_close(M);
    sem_close(semPagos);
    sem_close(confirmaProdPagos);
    sem_close(hayMemoriaParaLeer);

    sem_unlink("/M");
    sem_unlink("/semPagos");
    sem_unlink("/confirmaProdPagos");
    sem_unlink("/hayMemoriaParaLeer");
    
}
