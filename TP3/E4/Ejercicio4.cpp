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
#include <sys/types.h> 
#include <wait.h>

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

pid_t control = -1;
pid_t registro = -1;
int punteroEscritura;
ofstream archivoSalida;
int punteroLectura;


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
    punteroEscritura = open("./fifo", O_WRONLY);

    while (1)
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
    //ofstream archivoSalida;
    t_datosProceso datosProceso;
    string mensaje;

    archivoSalida.open("InformaProcesos.txt", ios_base::app);

    mkfifo("./fifo", 0666);
    //int punteroLectura = open("./fifo", O_RDONLY);
    punteroLectura = open("./fifo", O_RDONLY);

    // leer fifo
    read(punteroLectura, &datosProceso, sizeof(datosProceso));

    while (1)
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
    close(punteroEscritura);
    close(punteroLectura);
    archivoSalida.close();
    unlink("./fifo");
    kill(control,SIGTERM);
    kill(registro,SIGTERM);
    exit(EXIT_SUCCESS);
}

// Ayuda
void mostrar_ayuda()
{
    cout << "AYUDA" << endl;
    cout << "El script detecta fugas de memoria, excesiva utilización de CPU o ambas por parte de los procesos" << endl;
	cout << "Para esto un proceso Control verifica mediante valores limites pasados, si algun proceso tiene algun exceso" << endl;
    cout << "Se deben pasar los valores límites por parámetro." << endl;
    cout << "El script recibe como parametro estos limites separados por espacio, los cuales deben estar comprendidos entre un valor entero ente 0 y 100" << endl;
	cout << "Una vez detectado el exceso a treves de un FIFO se le pasa los datos del proceso a un proceso Registro"<< endl;
	cout << "El proceso Registro recibirá los datos de los proceso y los registrará en un archivo "<< endl;
	cout << "Se creara un archivo.txt llamado InformaProcesos.txt donde se guardara los datos"<< endl;
    cout << "" << endl;
    cout << "---------------------------------------------------------------------------------------" << endl;
    cout << "SINTAXIS:" << endl;
    cout << "./Ejercicio4.exe valorLimiteCPU valorLimiteMEM" << endl;
    cout << "" << endl;
    cout << "---------------------------------------------------------------------------------------" << endl;
    cout << "Se debe ejecutar makefile para compilar el archivo" << endl;
    cout << "Para finalizar la ejeucion del proceso principal utilizar kill -10 <PID>" << endl;
    cout << "" << endl;
}

/*Proceso principal*/
int main(int argc, char *argv[])
{
    int valorLimiteCPU = 0;
    int valorLimiteMEM = 0;

	//Validaciones
    if (argc == 2){
        if(!strcmp(argv[1], "-help") || !strcmp(argv[1], "-h") || !strcmp(argv[1], "-?")){
            mostrar_ayuda();
            return EXIT_FAILURE;
       }
    }       

    if (argc != 3)
    {
        cout << "Error: Se deben pasar 2 parametros" << endl;
        return EXIT_FAILURE;
    }

    if (!regex_match(argv[1], regex("([0-9]+)")))
    {
        cout << "Error: El primer parametro debe ser un valor entero comprendido entre 0 y 100" << endl;
        cout << "" << endl;
        return EXIT_FAILURE;
    }

    if (!regex_match(argv[2], regex("([0-9]+)")))
    {
        cout << "Error: El segundo parametro debe ser un valor entero comprendido entre 0 y 100" << endl;
        cout << "" << endl;
        return EXIT_FAILURE;
    }

    valorLimiteCPU = atoi(argv[1]);
    valorLimiteMEM = atoi(argv[2]);

    if (valorLimiteCPU < 0 || valorLimiteCPU > 100 || valorLimiteMEM < 0 || valorLimiteMEM > 100)
    {
        cout << "Error: Los valores esperados de los parametros son un valor entero comprendido entre 0 y 100" << endl;
        cout << "" << endl;
        return EXIT_FAILURE;
    }

    pid_t principal;
    
    if ( (principal=fork()) == 0 )
    {
        signal(SIGUSR1, signal_handler);
        cout << "El proceso principal tiene el siguiente PID: " << getpid() << endl;
     
        if ( (control=fork()) == 0 )
        { /* hijo 1*/   
            controla(valorLimiteCPU, valorLimiteMEM);     
            sleep(1);
        }
        else
        { 
            if ( (registro=fork()) == 0 )
            { /* hijo 2 */
                registra();
                sleep(1);
            }
        }
    }

    if(principal > 0 )
    {
        exit(1);
    }

    sleep(1500);
    return EXIT_SUCCESS;
}

// +++++++++++++++FIN DE ARCHIVO+++++++++++++++++++++++