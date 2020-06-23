/*-----------------------ENCABEZADO------------------------------------------------------------------------

 Nombre del script: Ejercicio6.cpp
 Trabajo practico: 3
 Ejercicio: 5
 Entrega: 1ra
 Integrantes:
	    Daiana Gomez Nespola, DNI 38005120
	    Juan Masi, DNI 37981647
        Guido Delbo, DNI 36982261
	    Nicolas Fortunato, DNI 37864810
	    Damian Perez, DNI 35375255

 ----------------------FIN ENCABEZADO---------------------------------------------------------------------

 --------------------------SCRIPT-------------------------------------------------------------------------*/

#include "funciones.h"


void getHelp()
{
    printf("El objetivo del script es comunicarse con un proceso Servidor\n");

    printf("con una base de datos cargada.\n");

	printf("Al iniciar el programa, el mismo se comunicará con el servidor\n"); 

	printf("Debe ingresar Usuario y contrasenia\n");

	printf("El programa cuenta con 2 funcionalidades posibles segun el ROL\n");

	printf("Si es DOCENTE, las funcionalidades son las siguientes:\n");

    printf("Consultar lista de asistencia: ingresa una fecha, y el servidor devolverá la asistencia de todos los alumnos \n");
	
	printf("Cargar asistencia: Lista los alumnos ingresando si esta presente (P) o ausente (A) y el servidor creara un nuevo archivo\n");
	
	printf("\n");
	
	rintf("Si es ALUMNO, las funcionalidades son las siguientes:\n");
	
	printf("Consultar asistencia: ingresa una fecha, y el servidor devolverá si asistio o no esa fecha \n");
	
	printf("Calcular porcentaje asistencia: calculara el porcentaje de asistencia e inasistencia\n");
	
	printf("\n");
    
	printf("\nPARAMETROS\n");
    
	printf("\nIP: Direccion IP del servidor\n");
    
	printf("PUERTO: Puerto del servidor\n");
    
	printf("\nEXAMPLE:\n");
    
	printf("./Cliente 192.168.0.53 8080\n");
    
	exit(1);
}
