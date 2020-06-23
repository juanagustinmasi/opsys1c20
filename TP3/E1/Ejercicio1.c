//  -----------------------ENCABEZADO------------------------------------------------------------------------

//  Trabajo practico: 3
//  Ejercicio: 1
//  Entrega: 1ra
//  Integrantes:
// 	    Daiana Gomez Nespola, DNI 38005120
// 	    Juan Masi, DNI 37981647
//      Guido Delbo, DNI 36982261
// 	    Nicolas Fortunato, DNI 37864810
// 	    Damian Perez, DNI 35375255

//  ----------------------FIN ENCABEZADO---------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/prctl.h>
#include <limits.h>

#define TODO_BIEN 1
#define SIN_MEMORIA 2
#define HAY_DUPLICADO 3
#define TAM_INFO 500

typedef struct sNodo
{
    char info[TAM_INFO];
    struct sNodo *sig;
} tNodo;

typedef tNodo *tLista;

int paramValido(const char *);
void crearLista(tLista *);
int listaLlena(const tLista *);
int listaVacia(const tLista *);
int insertarEnLista(tLista *, const char *);
void vaciarLista(tLista *);
void mostrarListaString(const tLista *);

int main(int argc, char *argv[])
{
    // ayuda y validaciones
    if (argc == 2)
    {
        if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-help") == 0)
        {
            printf("\nEste programa recibe un parámetro numérico 'n' y crea 'n' hijos del proceso 7 del siguiente grafo:\n\n\n");
            printf("\t\t \t0\n\n");
            printf("\t\t1\t2\t3\n\n");
            printf("\t\t4\t5\t6\n\n");
            printf("\t\t \t7\n\n\n");
            printf("\nEjemplo de ejecución: \"./Ejercicio1 2\"\n");
            exit(1);
        }
        else
        {
            if (!paramValido(argv[1]))
            {
                printf("El parametro ingresado no es válido.\n");
                printf("Consulte la ayuda con `-?` o `-help`\n");
                exit(2);
            }
        }
    }
    else
    {
        printf("Error, la cantidad de parametros ingresados no es correcta.\n");
        printf("Consulte la ayuda con `-?` o `-help`\n");
        exit(3);
    }

    char cad[TAM_INFO];
    int pid_act;
    int cant_hijos = atoi(argv[1]);
    int num_proceso = 0;
    tLista lista;

    crearLista(&lista);

    pid_act = getpid();

    snprintf(cad, TAM_INFO, "%d (%d)", num_proceso, pid_act);
    insertarEnLista(&lista, cad);
    mostrarListaString(&lista);

    // proceso 0
    for (int i = 0; i < 3; i++)
    {
        num_proceso++;
        // procesos 1, 2 y 3
        if (fork() == 0)
        {
            char cad[TAM_INFO];
            pid_act = getpid();
            snprintf(cad, TAM_INFO, "%d (%d)", num_proceso, pid_act);
            // info = cad;
            insertarEnLista(&lista, cad);
            mostrarListaString(&lista);

            if (i == 1)
            {
                num_proceso++;
                // proceso 4 y 5
                for (int l = 0; l < 2; l++)
                {
                    num_proceso++;
                    if (fork() == 0)
                    {
                        char cad[TAM_INFO];
                        pid_act = getpid();
                        snprintf(cad, TAM_INFO, "%d (%d)", num_proceso, pid_act);
                        insertarEnLista(&lista, cad);
                        mostrarListaString(&lista);

                        // proceso 7
                        if (l == 0)
                        {
                            if (fork() == 0)
                            {
                                char cad[TAM_INFO];
                                num_proceso += 3;
                                pid_act = getpid();
                                snprintf(cad, TAM_INFO, "%d (%d)", num_proceso, pid_act);
                                insertarEnLista(&lista, cad);
                                mostrarListaString(&lista);

                                // jerarquía debajo del proceso 7
                                while (cant_hijos > 0)
                                {
                                    if (fork() != 0)
                                        break;

                                    char cad[TAM_INFO];
                                    num_proceso++;
                                    pid_act = getpid();
                                    snprintf(cad, TAM_INFO, "%d (%d)", num_proceso, pid_act);
                                    insertarEnLista(&lista, cad);
                                    mostrarListaString(&lista);
                                    cant_hijos--;
                                }

                                break;
                            }
                        }
                        break;
                    }
                }
            }

            if (i == 2)
            {
                // proceso 6
                if (fork() == 0)
                {
                    char cad[TAM_INFO];
                    num_proceso += 3;
                    pid_act = getpid();
                    snprintf(cad, TAM_INFO, "%d (%d)", num_proceso, pid_act);
                    insertarEnLista(&lista, cad);
                    mostrarListaString(&lista);

                    break;
                }
            }

            break;
        }
    }

    sleep(5);
    return 0;
}

int paramValido(const char *s)
{
    while (*s)
    {
        if (isdigit(*s++) == 0)
            return 0;
    }

    return 1;
}

void crearLista(tLista *p)
{
    *p = NULL;
}

int listaLlena(const tLista *p)
{
    void *aux = malloc(sizeof(tNodo));

    free(aux);

    return aux == NULL;
}

int listaVacia(const tLista *p)
{
    return *p == NULL;
}

int insertarEnLista(tLista *p, const char *d)
{
    tNodo *nue = (tNodo *)malloc(sizeof(tNodo));

    if (nue == NULL)
        return SIN_MEMORIA;

    strcpy(nue->info, d);
    nue->sig = *p;
    *p = nue;

    return TODO_BIEN;
}

void vaciarLista(tLista *p)
{
    tNodo *aux;

    while (*p)
    {
        aux = *p;

        *p = aux->sig;

        free(aux);
    }
}

void mostrarListaString(const tLista *p)
{
    while (*p)
    {
        printf("%s", (*p)->info);
        if ((*p)->sig != NULL)
            printf(" - ");
        p = &(*p)->sig;
    }
    printf("\n");
}

//  ------------------------------FIN------------------------------------------------------------------------
//  SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
//  ---------------------------------------------------------------------------------------------------------