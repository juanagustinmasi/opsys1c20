#ifndef LIBRERIAS_H_INCLUDED
#define LIBRERIAS_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>   
#include <sys/shm.h>   
#include <fcntl.h>     
#include <semaphore.h> 
#include <string.h>
#include <signal.h>

typedef struct
{
    int dia;
    int mes;
    int anio;
} t_fecha;

typedef struct
{
    t_fecha fechaPago;
    float montoPagado;
} t_pago;

typedef struct
{
    char diaAsistencia[10];
} t_asistencia;

typedef struct
{
    char nombre[50];
    char apellido[50];
    char deporte[20];
    char diaInscripto[20]; 
    long dni; 
    t_pago pago;
    t_asistencia asistencia;
} t_socio;

typedef struct 
{
    long dni;
    char diaAsistencia[10];
    t_fecha fechapago;
} t_asistencia_pago;


#define OK              0
#define ERROR_LISTA     1
#define ERROR_ARCHIVO   2
#define NO_ENCONTRADO   3
#define ERROR_KEY       4
#define LISTA_VACIA     -1
#define DNI_NO_ENCONTRADO -2
#define ARCHIVO_SOCIOS  "Socios.txt"
#define ARCHIVO_PAGOS  "Pagos.txt"
#define ARCHIVO_ASISTENCIA  "Asistencia.txt"
#define CANT_MAX_SOCIOS     50

int actualiza_pago(t_socio arraySocios[CANT_MAX_SOCIOS], long dni, t_fecha fecha){
    for (int i = 0; i < CANT_MAX_SOCIOS; i++)
    {
        if(arraySocios[i].dni == dni){
            if(strcmp(arraySocios[i].deporte,"Futbol")==0)
                arraySocios[i].pago.montoPagado=1000;      
            else if(strcmp(arraySocios[i].deporte,"Basquet")==0)
                arraySocios[i].pago.montoPagado=1300; 
            else if(strcmp(arraySocios[i].deporte,"Voley")==0)
                arraySocios[i].pago.montoPagado=1200;   
            else if(strcmp(arraySocios[i].deporte,"Natacion")==0)
                arraySocios[i].pago.montoPagado=1800;          
            if(fecha.dia < 11)
                arraySocios[i].pago.montoPagado*=0.9; 
            return OK;
        }
    }
    return -1;
}

int actualiza_asistencia(t_socio arraySocios[CANT_MAX_SOCIOS], long dni, char diaAsistencia[10]){
    for (int i = 0; i < CANT_MAX_SOCIOS; i++)
    {
        if(arraySocios[i].dni == dni){
            strcpy(arraySocios[i].asistencia.diaAsistencia,diaAsistencia);
            return OK;
        }
    }
    return -1;
}


#endif // LIBRERIAS_H_INCLUDED