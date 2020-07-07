//  -----------------------ENCABEZADO------------------------------------------------------------------------

//  Trabajo practico: 3
//  Ejercicio: 4
//  Entrega: 2da
//  Integrantes:
// 	    Daiana Gomez Nespola, DNI 38005120
// 	    Juan Masi, DNI 37981647
//      Guido Delbo, DNI 36982261
// 	    Nicolas Fortunato, DNI 37864810
// 	    Damian Perez, DNI 35375255
//
//  ----------------------FIN ENCABEZADO---------------------------------------------------------------------
#include <stdio.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <csignal>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <fstream>
#include <regex>

using namespace std;

#define VAL_CPU 1
#define VAL_MEM 2
#define VAL_AMBOS 3

typedef struct
{
    int pid;
    float cpu;
    float mem;
    string nombre;
} t_proceso;

typedef struct
{
    int pid;
    char nombre[254];
    int tipoExceso;
    char hora[10];
} t_datosProceso;

int ejecuta = 1;

/*Hilo control*/
void parseoProceso(string linea, t_proceso *proceso)
{
    size_t pos = 0;
    const string separador = " ";

    // unificando los espacios que tenga
    string::iterator nuevoFin = unique(linea.begin(), linea.end());
    linea.erase(nuevoFin, linea.end());

    linea.erase(0, 1);

    pos = linea.find(separador);
    proceso->pid = atoi(linea.substr(0, pos).data());
    linea.erase(0, pos + separador.length());

    pos = linea.find(separador);
    proceso->cpu = atof(linea.substr(0, pos).data());
    linea.erase(0, pos + separador.length());

    pos = linea.find(separador);
    proceso->mem = atof(linea.substr(0, pos).data());
    linea.erase(0, pos + separador.length());

    pos = linea.find(separador);
    proceso->nombre = linea.substr(0, pos);
    proceso->nombre.erase(remove(proceso->nombre.begin(), proceso->nombre.end(), '\n'), proceso->nombre.end());
}

int BuscaFugaExceso(t_proceso *proceso, map<int, int> &map_proceso, int valorLimiteCPU, int valorLimiteMEM)
{
    int tipoExceso = 0, excesoRegistrado = 0;

    if (proceso->cpu > valorLimiteCPU && proceso->mem > valorLimiteMEM)
    {
        tipoExceso = VAL_AMBOS;
    }
    else if (proceso->cpu > valorLimiteCPU)
    {
        tipoExceso = VAL_CPU;
    }
    else if (proceso->mem > valorLimiteMEM)
    {
        tipoExceso = VAL_MEM;
    }

    excesoRegistrado = map_proceso[proceso->pid];

    if (excesoRegistrado == 0 && tipoExceso != 0)
    {
        map_proceso[proceso->pid] = tipoExceso;
        return tipoExceso;
    }
    else if (((excesoRegistrado == VAL_CPU && tipoExceso == VAL_MEM)) ||
             ((excesoRegistrado == VAL_MEM && tipoExceso == VAL_CPU)) ||
             ((excesoRegistrado != VAL_AMBOS && tipoExceso == VAL_AMBOS)))
    {
        map_proceso[proceso->pid] = VAL_AMBOS;
        return VAL_AMBOS;
    }

    return 0;
}

string horaSistemaActual()
{
    time_t horaActual = time(0);
    char hora[10] = "";
    strftime(hora, 10, "%H:%M:%S", localtime(&horaActual));

    return string(hora);
}

void controla(int valorLimiteCPU, int valorLimiteMEM)
{
    FILE *archivo;
    char buffer[5000];
    string cadena = "";
    string horaSistema;
    t_proceso proceso;
    map<int, int> registroProcesos;
    int resultado = 0;
    t_datosProceso datosProceso;
    string comando = "ps -e -o pid,%cpu,%mem,cmd | awk '$2 > " + to_string(valorLimiteCPU) + " || $3 > " + to_string(valorLimiteMEM) + " {print $0}'";

    mkfifo("./fifo", 0666);
    int punteroEscritura = open("./fifo", O_WRONLY);

    while (ejecuta)
    {
        archivo = popen(comando.data(), "r");
        //Verfico si abrio bien
        if (!archivo)
        {
            return;
        }

        horaSistema = horaSistemaActual();

        while (!feof(archivo))
        {

            if (fgets(buffer, 5000, archivo) != NULL)
            {
                cadena = buffer;
                parseoProceso(cadena, &proceso);

                resultado = BuscaFugaExceso(&proceso, registroProcesos, valorLimiteCPU, valorLimiteMEM);

                if (resultado != 0)
                {
                    datosProceso.tipoExceso = resultado;
                    strcpy(datosProceso.hora, horaSistema.data());
                    strcpy(datosProceso.nombre, proceso.nombre.data());
                    datosProceso.pid = proceso.pid;

                    // escribir fifo
                    write(punteroEscritura, &datosProceso, sizeof(datosProceso));
                }
            }
        }
        pclose(archivo);

        sleep(1);
    }

    // escribir fifo
    write(punteroEscritura, &datosProceso, sizeof(datosProceso));

    close(punteroEscritura);
    unlink("./fifo");
}

/*Hilo registro*/
void registra()
{
    ofstream archivoSalida;
    t_datosProceso datosProceso;
    string mensaje;

    archivoSalida.open("InformaProcesos.txt", ios_base::app);

    mkfifo("./fifo", 0666);
    int punteroLectura = open("./fifo", O_RDONLY);

    // leer fifo
    read(punteroLectura, &datosProceso, sizeof(datosProceso));

    while (ejecuta)
    {
        mensaje = datosProceso.tipoExceso == 1 ? "Supera CPU" : datosProceso.tipoExceso == 2 ? "Supera memoria" : "Supera ambos";

        // escribir archivo
        archivoSalida << datosProceso.hora << " "
                      << "${" << datosProceso.pid << "}: " << mensaje << " " << datosProceso.nombre << "." << endl;

        // leer fifo
        read(punteroLectura, &datosProceso, sizeof(datosProceso));
    }

    //cierro
    close(punteroLectura);
    archivoSalida.close();
}

/*Manejador de la senal*/
void signal_handler(int num_sig)
{
    ejecuta = 0;
}

// Ayuda
void mostrar_ayuda()
{
    cout << "Este programa verifica el porcentaje de utilización de memoria y CPU de los procesos corriendo en el sistema." << endl;
	cout << "Para esto un proceso Control verifica mediante valores límite recibidos por parámetro, si algún proceso los sobrepasa." << endl;
	cout << "Una vez detectado el exceso a trevés de un FIFO se le pasa los datos a un proceso Registro."<< endl;
	cout << "El proceso Registro guardará la información en el archivo InformaProcesos.txt.\n\n"<< endl;
    cout << "SINTAXIS:" << endl;
    cout << "./Ejercicio4 [CPU_LIMITE] [MEMORIA_LIMITE]\n\n" << endl;
    cout << "Los valores CPU_LIMITE y MEMORIA_LIMITE deben ser números enteros positivos menores a 100." << endl;
    cout << "El proceso principal se detiene con:  \"kill -10 <PID>\"" << endl;
}

/*Proceso principal*/
int main(int argc, char *argv[])
{
    int valorLimiteCPU = 0;
    int valorLimiteMEM = 0;

    //Validaciones
    if (argc == 2)
    {
        if (!strcmp(argv[1], "-help") || !strcmp(argv[1], "-h") || !strcmp(argv[1], "-?"))
        {
            mostrar_ayuda();
            return EXIT_FAILURE;
        }
    }

    if (argc != 3)
    {
        cout << "Para consultar la ayuda de este programa ingrese como parámetro: -h, -help o -?." << endl;
        return EXIT_FAILURE;
    }

    if (!regex_match(argv[1], regex("([0-9]+)")))
    {
        cout << "Error: El primer parametro debe ser un valor entero comprendido entre 0 y 100" << endl;
        return EXIT_FAILURE;
    }

    if (!regex_match(argv[2], regex("([0-9]+)")))
    {
        cout << "Error: El segundo parametro debe ser un valor entero comprendido entre 0 y 100" << endl;
        return EXIT_FAILURE;
    }

    valorLimiteCPU = atoi(argv[1]);
    valorLimiteMEM = atoi(argv[2]);

    if (valorLimiteCPU < 0 || valorLimiteCPU > 100 || valorLimiteMEM < 0 || valorLimiteMEM > 100)
    {
        cout << "Error: Los valores esperados de los parametros son un valor entero comprendido entre 0 y 100" << endl;
        return EXIT_FAILURE;
    }

    pid_t pid_principal = fork();

    //Verifico si se creo el proceso hijo
    if (pid_principal < 0)
    {
        printf("Error al querer crear un proceso hijo.\n");
        exit(1);
    }

    //Verifico que estoy en el padre.
    if (pid_principal > 0)
    {
        //Mato al proceso padre.
        exit(1);
    }

    signal(SIGUSR1, signal_handler);
    thread control(controla, valorLimiteCPU, valorLimiteMEM);
    thread registro(registra);

    cout << "El proceso principal tiene el siguiente PID: " << getpid() << endl;

    control.join();
    registro.join();

    return EXIT_SUCCESS;
}

//  ------------------------------FIN------------------------------------------------------------------------
//  SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
//  ---------------------------------------------------------------------------------------------------------