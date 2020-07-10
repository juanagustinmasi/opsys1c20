//  -----------------------ENCABEZADO------------------------------------------------------------------------

//  Trabajo practico: 3
//  Ejercicio: 3
//  Entrega: 3ra
//  Integrantes:
// 	    Daiana Gomez Nespola, DNI 38005120
// 	    Juan Masi, DNI 37981647
//      Guido Delbo, DNI 36982261
// 	    Nicolas Fortunato, DNI 37864810
// 	    Damian Perez, DNI 35375255

//  Se asume que no se tendra mas de 50 socios
//  ----------------------FIN ENCABEZADO---------------------------------------------------------------------

#include "librerias.h"

int main(int argc, char *argv[])
{

    // ayuda
    if (argc > 1)
    {
        if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-help") == 0)
        {
            printf("Este programa, a partir de tres archivos: uno de socios, otro de asistencias, y otro de pagos\n");
            printf("realiza el procesamiento necesario para mostrar por pantalla:\n");
            printf("    El monto total cobrado en el mes.\n");
            printf("    Asociados que no hayan pagado la cuota mensual.\n");
            printf("    Asistencias en dias que no corresponda.\n");
            printf("Este programa no recibe ningun parámetro.\n");
            printf("Ejemplo de ejecución: \"./Consumidor\"\n");
            return(OK);
        }
        else
        {
            printf("La cantidad de parámetros ingresados es incorrecta.\n");
            printf("Consulte la ayuda con `-?` o `-help`\n");
            return(ERROR_CANT_PARAMETROS);
        }
    }

    sem_t *M = sem_open("/M", O_CREAT, 0666, 0); // mutex para la memoria compartida
    sem_t *semPagos = sem_open("/semPagos", O_CREAT, 0666, 0); // semáforo productor pagos
    sem_t *semAsist = sem_open("/semAsist", O_CREAT, 0666, 0); // semáforo productor asistencias
    sem_t *confirmaProdPagos = sem_open("/confirmaProdPagos", O_CREAT, 0666, 0); //confirmacion del productor de Pagos
    sem_t *confirmaProdAsistencias = sem_open("/confirmaProdAsistencias", O_CREAT, 0666, 0); //confirmacion del productor de Asistencias
    sem_t *hayMemoriaParaLeer = sem_open("/hayMemoriaParaLeer", O_CREAT, 0666, 0); 

    key_t key = ftok(ARCHIVO_SOCIOS, 1);
    if (key == -1) {
        printf ("Error al obtener la Key \n");
        return(ERROR_KEY); 
    }  
    
    int shmid = shmget(key, sizeof(t_asistencia_pago), IPC_CREAT | 0666); // crea la memoria compartida

    t_asistencia_pago * pMemComp = (t_asistencia_pago*)shmat(shmid, NULL, 0); // asigna la memoria compartida a la estructura

    FILE * archBD=fopen(ARCHIVO_SOCIOS, "r");

    if( archBD == NULL)
    {
        printf("Error al acceder al archivo de socios\n");
        return(ERROR_ARCHIVO);
    }

    t_socio sociosArray[CANT_MAX_SOCIOS];

    int i = 0;
    fflush(stdin);
    while( fscanf(archBD,"%[^;];%[^;];%ld;%[^;];%s\n",sociosArray[i].nombre,sociosArray[i].apellido,&sociosArray[i].dni,sociosArray[i].deporte,sociosArray[i].diaInscripto) != EOF)
	{
        i++;
    }
    fclose(archBD);

    pid_t pidControl = -1;
    pid_t pidControl2 = -1;

    pidControl = fork();

    if(pidControl == 0) {
        FILE *archBD=fopen(ARCHIVO_PAGOS, "r");
        if( archBD == NULL)
        {
            printf("Error al acceder al archivo de pagos\n");
            return(ERROR_ARCHIVO);
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
    } else {

        pidControl2 = fork();
        if(pidControl2 == 0) {
            FILE *archBD=fopen(ARCHIVO_ASISTENCIAS, "r");
            if( archBD == NULL)
            {
                printf("Error al acceder al archivo de asistencias\n");
                return(ERROR_ARCHIVO);
            }

            //Utiliza la Memoria compartida
            while( !feof(archBD) )
            {
                sem_wait(semAsist);
                sem_wait(M);
                fflush(stdin);
                fscanf(archBD,"%ld;%s\n",&(*pMemComp).dni,(*pMemComp).diaAsistencia);

                pMemComp->fechapago.anio=0;
                pMemComp->fechapago.mes=0;
                pMemComp->fechapago.dia=0;
                strcpy(pMemComp->observaciones, "");

                sem_post(M);
                sem_post(hayMemoriaParaLeer);
            }
            fclose(archBD);

            sem_wait(semAsist);
            sem_wait(M);
            strcpy(pMemComp->observaciones, FIN_ARCHIVO_ASISTENCIAS);
            sem_post(M);
            sem_post(hayMemoriaParaLeer);

            sem_post(confirmaProdAsistencias);
        } else {
            //consumidor
            sem_post(semPagos);
            sem_post(M);/// V(M) Inicializa en 1 para los productores
            
            int finArchPagos = 0;
            int finArchAsistencias =0;
            time_t tiempo = time(NULL);
            struct tm *tlocal = localtime(&tiempo);

            int anioActual = 1900 + tlocal->tm_year; 
            int mesActual = 1 + tlocal->tm_mon;

            while(!(finArchPagos == 1 && finArchAsistencias == 1)){  
                sem_wait(hayMemoriaParaLeer);
                sem_wait(M);  

                //Se informo el fin de alguno de los archivos
                if(strcmp(pMemComp ->observaciones, FIN_ARCHIVO_ASISTENCIAS)== 0){
                    finArchAsistencias = 1;
                    sem_post(semPagos);
                }
                else{
                    if(strcmp(pMemComp ->observaciones, FIN_ARCHIVO_PAGOS)== 0){
                        finArchPagos = 1;
                        sem_post(semAsist);
                    }
                }

                if(strcmp(pMemComp->observaciones,"")==0){
                    //Si ya se termino de leer uno de los archivos, deberia de activar el otro para que no se quede esperando
                    if(finArchPagos == 1 && finArchAsistencias == 0){
                        sem_post(semAsist);
                    }
                    if(finArchPagos == 0 && finArchAsistencias == 1){
                        sem_post(semPagos);
                    }

                    if(strcmp(pMemComp->diaAsistencia,"")==0){
                        //Se guardo un pago
                        actualiza_pago(sociosArray, pMemComp->dni, pMemComp->fechapago, anioActual, mesActual);
                        //activo el prod de assistencias
                        sem_post(semAsist);
                    }else{
                        //Se guardo una asistencia
                        actualiza_asistencia(sociosArray, pMemComp->dni, pMemComp->diaAsistencia);
                        //Activo el prod de pagos
                        sem_post(semPagos);
                    }
                }
                sem_post(M);
            }
            //REALIZA LOS CALCULOS Y MUESTRA POR PANTALLA
            sem_wait(confirmaProdPagos);
            sem_wait(confirmaProdAsistencias);

            float montoTotal = 0;
            
            printf("**  Informes obtenidos:  **\n");
            printf("\n");
            for (int y = 0; y < i; y++)
            {
                montoTotal+=sociosArray[y].pago.montoPagado;
                //puede haber socios que no tengan asistencias
                if(sociosArray[y].asistencia.diaAsistencia != NULL && strcmp(sociosArray[y].asistencia.diaAsistencia, "") != 0 && strcmp(sociosArray[y].diaInscripto, sociosArray[y].asistencia.diaAsistencia ) != 0){
                    printf("El socio %s %s con DNI: %ld asistio el dia %s en lugar de %s a %s.\n", sociosArray[y].nombre, sociosArray[y].apellido, sociosArray[y].dni, sociosArray[y].asistencia.diaAsistencia, sociosArray[y].diaInscripto, sociosArray[y].deporte);
                }
            }
            printf("\n");
            for (int y = 0; y < i; y++)
            {
                if(sociosArray[y].pago.montoPagado == 0){
                    printf("El socio %s %s con DNI: %ld no abono la cuota del mes.\n", sociosArray[y].nombre, sociosArray[y].apellido, sociosArray[y].dni);
                }
            }
            printf("\n");
            printf("** Monto Total cobrado en el mes: $%.2f **\n",montoTotal);
        
        }
    }

    // libera los recursos
    shmdt(&shmid);
    sem_close(M);
    sem_close(semPagos);
    sem_close(semAsist);
    sem_close(confirmaProdPagos);
    sem_close(confirmaProdAsistencias);
    sem_close(hayMemoriaParaLeer);

    sem_unlink("/M");
    sem_unlink("/semPagos");
    sem_unlink("/semAsist");
    sem_unlink("/confirmaProdPagos");
    sem_unlink("/confirmaProdAsistencias");
    sem_unlink("/hayMemoriaParaLeer");
}

//  ------------------------------FIN------------------------------------------------------------------------
//  SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
//  ---------------------------------------------------------------------------------------------------------