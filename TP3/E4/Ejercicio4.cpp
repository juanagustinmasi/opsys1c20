//  -----------------------ENCABEZADO------------------------------------------------------------------------

//  Trabajo practico: 3
//  Ejercicio: 4
//  Entrega: 1ra
//  Integrantes:
// 	    Daiana Gomez Nespola, DNI 38005120
// 	    Juan Masi, DNI 37981647
//      Guido Delbo, DNI 36982261
// 	    Nicolas Fortunato, DNI 37864810
// 	    Damian Perez, DNI 35375255

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
bool unificaEspacios(char lhs, char rhs)
{
    return (lhs == rhs) && (lhs == ' ');
}

void parseoProceso(string linea, t_proceso *proceso)
{
    size_t pos = 0;
    const string separador = " ";

    // unificando los espacios que tenga
    string::iterator nuevoFin = unique(linea.begin(), linea.end(), unificaEspacios);
    linea.erase(nuevoFin, linea.end());

    linea.erase(0, 1);

    pos = linea.find(separador);
    proceso->pid = stoi(linea.substr(0, pos));
    linea.erase(0, pos + separador.length());

    pos = linea.find(separador);
    proceso->cpu = stof(linea.substr(0, pos));
    linea.erase(0, pos + separador.length());

    pos = linea.find(separador);
    proceso->mem = stof(linea.substr(0, pos));
    linea.erase(0, pos + separador.length());

    pos = linea.find(separador);
    proceso->nombre = linea.substr(0, pos);
    proceso->nombre.erase(remove(proceso->nombre.begin(), proceso->nombre.end(), '\n'), proceso->nombre.end());
}

int verificaExceso(t_proceso *proceso, map<int, int> &map_proceso, int valorLimiteCPU, int valorLimiteMEM)
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

string hora_del_sistema()
{
    time_t horaActual = time(0);
    char hora[10] = "";
    strftime(hora, 10, "%H:%M:%S", localtime(&horaActual));

    return string(hora);
}

void controlaProcesos(int valorLimiteCPU, int valorLimiteMEM)
{
    t_proceso proceso;
    map<int, int> procesosRegistrados;
    int resultado = 0;
    FILE *archivo;
    char buffer[5000];
    string cadena = "", horadelsistema;
    t_datosProceso datosProceso; 

    string comando = "ps -e -o pid,%cpu,%mem,cmd | awk '$2 > " + to_string(valorLimiteCPU) + " || $3 > " + to_string(valorLimiteMEM) + " {print $0}'";

    mkfifo("./fifo", 0666);
    int punteroEscritura = open("./fifo", O_WRONLY);

    while (ejecuta)
    {
        archivo = popen(comando.c_str(), "r");
		//Verfico si abrio bien
        if (!archivo)
        {
            return;
        }

        horadelsistema = hora_del_sistema();

        while (!feof(archivo))
        {

            if (fgets(buffer, 5000, archivo) != NULL)
            {
                cadena = buffer;
                parseoProceso(cadena, &proceso);

                resultado = verificaExceso(&proceso, procesosRegistrados, valorLimiteCPU, valorLimiteMEM);

                if (resultado != 0)
                {
                    datosProceso.tipoExceso = resultado;
                    strcpy(datosProceso.hora, horadelsistema.c_str());
                    strcpy(datosProceso.nombre, proceso.nombre.c_str());
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
void registraProcesos()
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
void signal_handler(int signal_num)
{
    ejecuta = 0;
}

// Ayuda
void mostrar_ayuda()
{
    cout << "El script detecta fugas de memoria, excesiva utilización de CPU o ambas por parte de los procesos" << endl;
	cout << "Para esto un proceso Control verifica mediante valores limites pasados, si algun proceso tiene algun exceso" << endl;
    cout << "Se deben pasar los valores límites por parámetro." << endl;
    cout << "El script recibe como parametro: los limites separados por espacio [Valor entero ente 0 y 100]" << endl;
	cout << "Una vez detectado el exceso a treves de un FIFO se le pasa los datosProceso a un proceso Registro"<< endl;
	cout << "El proceso Registro recibirá los datos de los proceso y los registrará en un archivo "<< endl;
	cout << "Se creara un archivo.txt llamado InformaProcesos.txt donde se guardara los datos"<< endl;
    cout << "" << endl;
    cout << "---------------------------------------------------------------------------------------" << endl;
    cout << "SINTAXIS:" << endl;
    cout << "./Source.exe [CPU_LIMITE] [MEMORIA_LIMITE]" << endl;
    cout << "" << endl;
    cout << "---------------------------------------------------------------------------------------" << endl;
    cout << "Ejecutar previamente \"makefile\" para compilar el archivo" << endl;
    cout << "Para detener el proceso principal utilizar \"kill -10 <PID>\"" << endl;
    cout << "" << endl;
}

/*Proceso principal*/
int main(int argc, char *argv[])
{
	//Validaciones
    if (argc == 2 && (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0))
    {
        mostrar_ayuda();
        return EXIT_FAILURE;
    }

    if (argc != 3)
    {
        cout << "El numero de parametros es incorrecto" << endl;
        return EXIT_FAILURE;
    }

    if (!regex_match(argv[1], regex("([0-9]+)")))
    {
        cout << "El primer parametro es erroneo" << endl;
        cout << "El primer parametro debe ser: [Un valor entero entre 0 y 100]" << endl;
        cout << "" << endl;
        return EXIT_FAILURE;
    }

    if (!regex_match(argv[2], regex("([0-9]+)")))
    {
        cout << "El segundo parametro es erroneo" << endl;
        cout << "El segundo parametro debe ser: [Un valor entero entre 0 y 100]" << endl;
        cout << "" << endl;
        return EXIT_FAILURE;
    }

    int valorLimiteCPU = stoi(argv[1]);
    int valorLimiteMEM = stoi(argv[2]);

    if (valorLimiteCPU < 0 || valorLimiteCPU > 100 || valorLimiteMEM < 0 || valorLimiteMEM > 100)
    {
        cout << "Error en los valores de los parametros" << endl;
        cout << "Los valores esperados de los parametros son: [Valor Entero entre 0 y 100]" << endl;
        cout << "" << endl;
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    //Valido que se creo bien el proceso hijo
    if (pid < 0)
    {
        printf("Error al querer crear un proceso hijo.\n");
        exit(1);
    }

    //Verifico que estoy en el padre. Mato al proceso padre.
    if (pid > 0)
    {
        exit(1);
    }

    signal(SIGUSR1, signal_handler);

    thread control(controlaProcesos, valorLimiteCPU, valorLimiteMEM);
    thread registro(registraProcesos);

    cout << "PID del proceso Principal: " << getpid() << endl;

    control.join();
    registro.join();
    
    return EXIT_SUCCESS;
}

// +++++++++++++++FIN DE ARCHIVO+++++++++++++++++++++++