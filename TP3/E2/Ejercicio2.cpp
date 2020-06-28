
/*-----------------------ENCABEZADO------------------------------------------------------------------------

 Nombre del script: Ejercicio6.cpp
 Trabajo practico: 3
 Ejercicio: 2
 Entrega: 1ra
 Integrantes:
	    Daiana Gomez Nespola, DNI 38005120
	    Juan Masi, DNI 37981647
        Guido Delbo, DNI 36982261
	    Nicolas Fortunato, DNI 37864810
	    Damian Perez, DNI 35375255

 ----------------------FIN ENCABEZADO---------------------------------------------------------------------

 --------------------------SCRIPT-------------------------------------------------------------------------*/


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <typeinfo>
#include <regex>

#define MAXVAL 1000
#define TODO_OK 1
#define ERROR 0
#define OVERFLOW -1

using namespace std;

double AcumSumaSucesion[MAXVAL];
long double AcumSumaProd[2] = {0, 1};

void mostrar_ayuda()
{       
    cout << "\033[1;32m_______________________________________________________________________\033[0m\n"<<endl;
    cout << "\033[1;32mScript Investigadores, sucesión de Fibonacci con Hilos\033[0m\n" << endl;
    cout << "\033[1;32mEl script recibe como parametro: -N (Entero Positivo)\033[0m\n" << endl;
    cout << endl;
    cout << "\033[1;32m.Ejemplo:\033[0m\n" << endl;
    cout << "\033[1;32m./Ejercicio2.exe -N (Entero Positivo)\033[0m\n" << endl;
    cout << endl;
    cout << "\033[1;32m________________________________________________________________________\033[0m\n" << endl;
    cout << endl;
}

class HiloFibonacci
{
    int numero;

public:
    // Constructor parametrizado
    HiloFibonacci(int numero){ this->numero = numero;}
    // Destructor
    ~HiloFibonacci() {}

    double getSumaSucesion()
    {
        double resultado = 1,
            ant = 0,
            sig = 0,
            sum = 1;

        for (int i = 1; i < numero; i++){
            ant = sig;
            sig = sum;
            sum = ant + sig;
            resultado += sum;
        }

        return resultado;
    }
};

void Hilo(int idThread)
{
    HiloFibonacci hf(idThread);
    AcumSumaSucesion[idThread - 1] = (hf.getSumaSucesion() * 1);
}

void HiloSuma(int num)
{
    for (int i = 0; i < num ; i++)
    {
        AcumSumaProd[0] +=  AcumSumaSucesion[i];
    }
}

void HiloProducto(int num)
{
    for (int i = 0; i < num ; i++)
    {
        AcumSumaProd[1] *=  AcumSumaSucesion[i];
    }
}

int main(int argc, char const *argv[])
{
    /******************************* VALIDACIONES ****************************************************/
    
    if(argc == 0 || argv[1] == NULL || argc != 3){  /* solucion segmentation fault por param */
        cout << "\033[1;31m ERROR_CANTIDAD_PARAMETROS \033[0m\n" << endl;
        mostrar_ayuda();
        return ERROR;
    }
    

    if (argc == 2 && !argv[1] || strcasecmp(argv[1], "-help") == 0 || (strcmp(argv[1], "-?") == 0 || strcasecmp(argv[1], "-h") == 0 ))
    {
        mostrar_ayuda();
        return TODO_OK;
    }

    if( atoi(argv[2]) >0 ){ 

        int parteEntera = (int)atoi(argv[2]);
        if (!regex_match(argv[2], regex("([0-9]+)")) || (atof(argv[2]) - parteEntera != 0) ){  /*chequeo que no se pase algo distinto a entero por param*/
        cout << "\033[1;31m ERROR_EN_SEGUNDO_PARAMETRO \033[0m\n" << endl;
        cout << "El segundo parametro esperado es un valor entero positivo sin decimales ni letras" << endl;
        cout << endl;
        mostrar_ayuda();
        return ERROR;
        }
    }

    if (strcasecmp(argv[1], "-N") != 0)
    {
        cout << "\033[1;31m ERROR_EN_PRIMER_PARAMETRO \033[0m\n" << endl;
        cout << "Se esperaba recibir -N" << endl;
        mostrar_ayuda();
        return ERROR;
    }
    if(!(atoi(argv[2]) >=0) && atoi(argv[2]) <= MAXVAL ){
        cout << "\033[1;31m ERROR_EN_SEGUNDO_PARAMETRO \033[0m\n" << endl;
        cout << "El segundo parametro esperado es un valor entero positivo" << endl;
        cout << endl;
        mostrar_ayuda();
        return ERROR;
    }

    unsigned int numParam = atoi(argv[2]);

    if (numParam == 0)
    {
        cout << "\033[1;31m ERROR_EN_SEGUNDO_PARAMETRO \033[0m\n" << endl;
        cout << "Por favor revisar ayuda" << endl;
        cout << endl;
        mostrar_ayuda();
        return ERROR;
    }
    if (numParam > MAXVAL)
    {   
        cout << "\033[1;31m STACK_OVERFLOW \033[0m\n"<< endl;
        cout << "Segundo parametro debe ser menor a " << MAXVAL << endl;
        mostrar_ayuda();
        return OVERFLOW;
    }
    
/*******************************************FIN VALIDACIONES***************************************/

    thread thSumaSucesion[numParam];

    for (int i = numParam; i >= 1; i--){
        thSumaSucesion[i-1] = thread(Hilo, i);
    }

    for (int i = numParam; i >= 1; i--){
        thSumaSucesion[i-1].join();
    }

    thread thSuma(HiloSuma, numParam);
    thread thProducto(HiloProducto, numParam);

    thSuma.join();
    thProducto.join();

    cout << AcumSumaProd[1] - AcumSumaProd[0] << endl;

    return TODO_OK;
    exit(1);
}


/*
# -----------------------FIN SCRIPT------------------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------# 
*/