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

char respuesta[TAM_QUERY];

int socketCliente;

int longitud;

t_request query;

t_usuario usuario;

void salir()
{
  

	printf("Finalizando el proceso, Por favor espere...");

	for(int i = 0;i < 2; i++){
        printf(".");
		sleep(3);
	}

	 printf("\n");

	query.operacion = 3;

    send(socketCliente, &query, sizeof(t_request), 0);

    close(socketCliente);
   
	exit(1);
}


void set(const char *ip, const char *puerto)
{
    bzero(&(configuracionSocket.sin_zero), 8);
    
	configuracionSocket.sin_family = AF_INET;
    
	configuracionSocket.sin_addr.s_addr = inet_addr(ip);
    
	configuracionSocket.sin_port = htons(atoi(puerto));
}

void operaciones(char *rol, int cod){
	
	char listaAsistencia[50];
	char c;
	
	if(strcmp(rol, "DOCENTE")==0){
		
		if(query.operacion == 1){

			fgetc(stdin);

			printf("\nIngrese fecha con formato YYYY-MM-DD: ");
			
			scanf("%[^\n]", query.linea);

			fgetc(stdin);
			
			send(socketCliente, &query, sizeof(t_request), 0);

			memset(respuesta, 0, strlen(respuesta));
			
			recv(socketCliente, respuesta, sizeof(int), 0);
			
			longitud = atoi(respuesta);
			
			recv(socketCliente, respuesta, longitud, 0);

			if(strstr(respuesta,"ERROR")!=NULL)
			{
				strcpy(respuesta,"ERROR");
			}
			
			if(strcmp(respuesta, "ERROR")!=0){

				sprintf(listaAsistencia, "\nAsistencia Comision: %d\n", cod);

				printf("%s", listaAsistencia);

				while (strcmp(respuesta, "FIN")!=0){

					printf("\t%s",respuesta);

					memset(respuesta, 0, strlen(respuesta));

					recv(socketCliente, respuesta, sizeof(int), 0);

					longitud = atoi(respuesta);

					recv(socketCliente, respuesta, longitud, 0);
				}
			}else{

				printf("\nArchivo inexistente");
			
			}

			printf("\nPresione cualquier tecla para volver al menu principal\n");

			fflush(stdin);

			scanf("%c",&c);

			system("clear");
			
		}
		
		if(query.operacion == 2){
				
			send(socketCliente, &query, sizeof(t_request), 0); 

			memset(respuesta, 0, strlen(respuesta));

			recv(socketCliente, respuesta, sizeof(int), 0);

			longitud = atoi(respuesta);

			recv(socketCliente, respuesta, longitud, 0);

			if(strstr(respuesta,"ERROR")!=NULL)
			{
				strcpy(respuesta,"ERROR");
			}
				
			if(strcmp(respuesta, "ERROR") != 0){
				
				printf("Ingrese la tecla P si está presente o A si está ausente");
				
				while (strcmp(respuesta, "FIN")!=0){
					
					printf("\nAlumno: %s \n", respuesta);	
					
					printf("Ingrese asistencia: ");	
					
					fgetc(stdin);
					
					scanf("%[^\n]", query.linea);
					
					send(socketCliente, &query, sizeof(t_request), 0); 
					
					memset(respuesta, 0, strlen(respuesta));
					
					recv(socketCliente, respuesta, sizeof(int), 0);
					
					longitud = atoi(respuesta);
					
					recv(socketCliente, respuesta, longitud, 0);
				
				}
				printf("\nSe ha creado el Archivo asistencias");
			
			}else{

				printf("\nYa se cargo un archivo para la FECHA de hoy\n");
			}

			printf("\n\t\tPara volver Presionar cualquier tecla\n");
			
			fgetc(stdin);
			
			scanf("%c",&c);

			system("clear");
		}
			
		
	}else {
		
		if(strcmp(rol, "ALUMNO")==0){

			if(query.operacion == 1){
				
				fgetc(stdin);
			
				printf("\nIngrese la fecha de asistencia con formato YYYY-MM-DD: ");
				
					
				
				scanf("%[^\n]", query.linea);
				
				fgetc(stdin);
				
				memset(respuesta, 0, strlen(respuesta));
				
				send(socketCliente, &query, sizeof(t_request), 0);
				
				recv(socketCliente, respuesta, sizeof(int), 0);
				
				longitud = atoi(respuesta);
				
				recv(socketCliente, respuesta, longitud, 0);
				
				printf("%s\n\n", respuesta);
				
				printf("Para volver Presionar cualquier tecla\n");
				
				fgetc(stdin);
				
				scanf("%c",&c);
				
				system("clear");
			}
			
			if(query.operacion == 2){
				
				send(socketCliente, &query, sizeof(t_request), 0);
				
				memset(respuesta, 0, strlen(respuesta));
				
				recv(socketCliente, respuesta, sizeof(int), 0);
				
				longitud = atoi(respuesta);
				
				recv(socketCliente, respuesta, longitud, 0); 
				
				printf("%s\n\n", respuesta);
				
				printf("Para volver Presionar cualquier tecla\n");
				
				fgetc(stdin);
				
				scanf("%c",&c);
				
				system("clear");
			}
		}
	}
	
}


int main(int argc, char *argv[])
{

	if (argc < 2)
    {
        printf("\nInserte los parametros necesarios\n");
        
		getHelp();
        
		exit(1);
    }
    
    if (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0)
    {
        getHelp();
    }
	
    signal(SIGINT, salir);
    
	signal(SIGTERM, salir);

    if ((socketCliente = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("SOCKET_CREATION_ERROR\n");
        return 1;
    }

    
    set(argv[1], argv[2]);

    if (connect(socketCliente, 
				(struct sockaddr *)&configuracionSocket,
				 sizeof(configuracionSocket)) == -1)
    {

        printf("CONECTION_REJECTED\n");
        return 1;
    }
	
	printf("Ingrese Nombre de Usuario: ");
	
	fflush(stdin);
	
	scanf("%[^\n]", usuario.usuario);
	
	fgetc(stdin);
	
	printf("\nIngrese Contraseña:");
	
	fflush(stdin);

	scanf("%[^\n]", usuario.password);
	
	fgetc(stdin);
	
	send(socketCliente, &usuario, sizeof(t_usuario), 0); 
	
    recv(socketCliente, respuesta, sizeof(int), 0); 
	
	longitud = atoi(respuesta);
    
	recv(socketCliente, respuesta, longitud, 0); 
	
	while(strcmp(respuesta, "ERROR") == 0) {
		
		printf("Usuario o Conraseña incorrecto\n");
		
		printf("Ingrese Usuario: ");
		
		fflush(stdin);
		
		scanf("%[^\n]", usuario.usuario);
		
		fgetc(stdin);
		
		printf("\nIngrese Contraseña: ");
		
		fflush(stdin);
		
		scanf("%[^\n]", usuario.password);
		
		fgetc(stdin);
		
		memset(respuesta, 0, sizeof(respuesta));
		
		send(socketCliente, &usuario, sizeof(t_usuario), 0);
		
		recv(socketCliente, respuesta, sizeof(int), 0);
		
		longitud = atoi(respuesta);
		
		recv(socketCliente, respuesta, longitud, 0);
	}
	
	char *token;
	
	char delimitador[] = "=\n";
	
	char rol[10];
	
	int comision;
	
	token = strtok(respuesta, delimitador);
	
	if(token !=NULL){
	
		strcpy(rol, token);	
	}
		
	token = strtok(NULL, delimitador);
	
	if(token !=NULL){
	
		comision = atoi(token);	
	
	}
	
	token = NULL;
	
	system("clear");
	
	printf("\n\t\tBIENVENIDO %s %s\n\n", rol, usuario.usuario);
	
	query.operacion = 0;
	
	while(query.operacion != 3){

		printf("\t\t**************************** MENU PRINCIPAL *******************************\n\n");
		
		if(!strcmp(rol, "DOCENTE")){

			printf("1- Consultar Asistencias \n");

			printf("2- Cargar Asistencias \n");

			printf("3- EXIT \n");	

			printf("\t\t************************************************************************\n\n");
		}

		else{ 
			printf("1. Consultar asistencias \n");

			printf("2. Calcular porcentaje asistencias \n");

			printf("3. EXIT \n");

			printf("\t\t************************************************************************\n\n");
		}
		
        printf("Ingrese el nro de operación que desea realizar: ");

        scanf("%d", &query.operacion);

		system("clear");

        if(query.operacion < 1 || query.operacion > 3){

            printf("Ingreso incorrecto. Ingrese nuevamente.\n\n");

			system("clear");
        }

        else if(query.operacion < 3)
        {
			operaciones(rol, comision);
            
        }
        
    }
	send(socketCliente, &query, sizeof(t_request), 0);


	printf("Finalizando el proceso, Por favor espere...");
	sleep(3);
    close(socketCliente);
    
	exit(1);
		
}
