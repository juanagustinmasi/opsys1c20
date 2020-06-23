#include "funciones.h"
/*
***********************************************************************************
 Nombre Del Script:        Cliente
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

void getHelp()
{
    printf("\nEjercicio 5 (SOCKET)\n================================\n\n");
    printf("El fin del programa es comunicarse con un proceso Servidor\n");
    printf("el cual tiene una base de datos cargada.\n");
	printf("Al iniciar el programa, el mismo se comunicará con el servidor\n"); 
	printf("Debe ingresar Usuario y Password\n");
	printf("El programa cuenta con 2 funcionalidades posibles segun el ROL\n");
	printf("Si es DOCENTE, las funcionalidades son las siguientes:\n");
    printf("[1] Consultar lista de asistencia \n");
	printf("[2] Cargar asistencia \n");
	printf("[3] SALIR \n\n");
	printf("La funcion 1 simplemente ingresa una fecha, y el servidor devolverá la asistencia de todos los alumnos\n");
	printf("La funcion 2 le listará a los alumnos de a una a la vez y tendrá que ingresar si esta presente o no y el servidor creara un nuevo archivo\n\n");	
	printf("Si es ALUMNO, las funcionalidades son las siguientes:\n");
	printf("[1] Consultar asistencia \n");
	printf("[2] Calcular porcentaje asistencia/inasistencia \n");
	printf("[3] SALIR \n\n");
	printf("La funcion 1 ingresa una fecha, y el servidor devolverá si asistio o no ese dia\n");
	printf("La funcion 2 calculara el porcentaje de asistencia e inasistencia\n\n");
    printf("\nPARAMETROS\n==========\n");
    printf("\n\nIP: Direccion IP del servidor\n");
    printf("PUERTO: Puerto del servidor\n");
    printf("\n\nEJEMPLOS\n========\n");
    printf("\t\t./Cliente 192.168.1.21 9999\n");

    exit(1);
}
