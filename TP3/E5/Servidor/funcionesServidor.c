#include "funciones.h"
/*
***********************************************************************************
 Nombre Del Script:        Servidor
 Trabajo Practico Nro.:    3
 Ejercicio Nro.:           5
 Entrega Nro.:             1
 Integrantes:
    Apellido            Nombre                  DNI
    --------------------------------------------------
   Fernández		    Jonathan		      37226233
   Suriano				Lucas Emanuel		  40429973
   Cicarone				Florencia 			  40712842
   Muñoz				Rocio Celeste		  39788890
   Sosa					Melisa Agustina 	  40464205
***********************************************************************************
*/
char bd[512];
void *obtenerQuery(void *sockfdVoid)
{
    int sockfd = *((int *)sockfdVoid);
    FILE *arch = fopen(bd, "rt");
	t_usuario usuario; //Para iniciar sesion.
	t_datos usuarioLoggeado; //Para guardar los datos del usuario que inicio sesion.
	t_request query; //Para recibir la consulta.
	int habilitado = 0;
	
    recv(sockfd, &usuario, sizeof(t_usuario), 0); //Recibo Usuario y Password.
	
	habilitado = iniciarSesion(arch, sockfd, &usuario, &usuarioLoggeado);
	
	while(habilitado == 0){ //Si ingreso un usuario que no existe.
		recv(sockfd, &usuario, sizeof(t_usuario), 0);
		habilitado = iniciarSesion(arch, sockfd, &usuario, &usuarioLoggeado);
	}
	
	recv(sockfd, &query, sizeof(t_request), 0); //Recibo la operacion.
	
	while(query.operacion != 3) {
		
		operaciones(arch, sockfd, &query, &usuarioLoggeado);
		recv(sockfd, &query, sizeof(t_request), 0);
	}

	char msjDesconexion[100];
	sprintf(msjDesconexion, "[CONEXION TERMINADA] El usuario %s se ha desconectado\n", usuarioLoggeado.nombre);
	printf("%s", msjDesconexion);
	log_message(LOG_FILE, msjDesconexion);
    fflush(stdout);

    fclose(arch);
    close(sockfd);
}

/////////////////////////////////////////////////////////////////////////////

int iniciarSesion(FILE *arch, int socketCliente, t_usuario *query, t_datos *registro){
	
	char linea[100];
	char delimitador[] = "|\n";
	char *token;
	char docente[20];
	char alumno[20];
	char tamPaquete[10];
	
	fseek(arch, 0, SEEK_SET);
	fgets(linea, 100, arch);
	fgets(linea, 100, arch);
	
	while(!feof(arch)){
		
		token = strtok(linea, delimitador);
		if(token !=NULL){
			strcpy(registro->nombre, token);	
		}
		
		token = strtok(NULL, delimitador);
		if(token !=NULL){
			strcpy(registro->password, token);	
		}
		token = strtok(NULL, delimitador);
		if(token !=NULL){
			strcpy(registro->rol, token);	
		}
		token = strtok(NULL, delimitador);
		if(token !=NULL){
			registro->cod_comision = atoi(token);
				
		}
			
		if(strcmp(registro->nombre, query->usuario) == 0 && strcmp(registro->password, query->password) == 0){
			
			if(strcmp(registro->rol,"D") == 0){
				sprintf(docente, "DOCENTE=%d", registro->cod_comision);
				enviarMensaje(docente, socketCliente, tamPaquete);
				usleep(1 * 1000);
			}else{
				sprintf(alumno, "ALUMNO=%d", registro->cod_comision);
				enviarMensaje(alumno, socketCliente, tamPaquete);
				usleep(1 * 1000);
			}
			
			char loggeado[100];
			sprintf(loggeado, "El usuario %s ha iniciado sesion.", query->usuario);
			printf("%s\n", loggeado);
			log_message(LOG_FILE, loggeado);
			fflush(stdout);
			
			return INICIADO;
		}

		fgets(linea, 100, arch);
		token = NULL;
	}
	const char error[] = "ERROR";
	enviarMensaje(error, socketCliente, tamPaquete);
	usleep(1 * 1000);
	return NO_ENCONTRADO;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void operaciones(FILE *arch, int socketCliente, t_request *query, t_datos *usuarioLoggeado) {
	
	char lineas[100];
	char delimitador[] = "|\n";
	char tamPaquete[10];
	char nombreAsistencia[100]; //archivo asistencia;
	
	
	if(strcmp(usuarioLoggeado->rol,"D") == 0){
		///EL DOCENTE CONSULTA EL ARCHIVO DE ASISTENCIA EN DETERMINADA FECHA.
		if(query->operacion == 1){
			///FORMO EL NOMBRE DEL ARCHIVO.
			snprintf(nombreAsistencia, sizeof(nombreAsistencia), "%sAsistencia_%s_%d.txt" , DIR_ASISTENCIA, query->linea, usuarioLoggeado->cod_comision);
			mostrarYGuardarMSJ("El cliente solicita LISTAR ASISTENCIA");		
			///VERIFIVO SI EXISTE EL ARCHIVO.
			if (access(nombreAsistencia, F_OK) != -1){
				mostrarYGuardarMSJ("El Servidor envia");
				FILE *archAsistencia = fopen(nombreAsistencia, "rt"); //Abro el archivo.
				fgets(lineas, 100, archAsistencia); //Leo toda la linea del archivo.
				while(!feof(archAsistencia)){
					enviarMensaje(lineas, socketCliente, tamPaquete); //envio cada linea al cliente. NOMBRE|PRESENTE
					mostrarYGuardarMSJ(lineas);
					usleep(1 * 1000);
					fgets(lineas, 100, archAsistencia);
					
				}
				fclose(archAsistencia);
				const char fin[] = "FIN";
				enviarMensaje(fin, socketCliente, tamPaquete);
				
			}else{
				mostrarYGuardarMSJ("El Servidor envia ARCHIVO INEXISTENTE.");
				const char error[] = "ERROR";
				enviarMensaje(error, socketCliente, tamPaquete);
				
			}	
				
		}
		///EL DOCENTE CREA UN NUEVO ARCHIVO DE ASISTENCIA.	
		if(query->operacion == 2){
			mostrarYGuardarMSJ("El Cliente solicita crear ASISTENCIA.");
			char *token;
			t_datos alumnos;
			time_t tiempo = time(NULL);
			struct tm *tiempoLocal = localtime(&tiempo);
			char fecha[20];
			///OBTENGO LA FECHA ACTUAL.
			strftime(fecha, sizeof(fecha), "%Y-%m-%d" , tiempoLocal);
			///FORMO EL NOMBRE DEL ARCHIVO.
			snprintf(nombreAsistencia, sizeof(nombreAsistencia), "%sAsistencia_%s_%d.txt" , DIR_ASISTENCIA, fecha, usuarioLoggeado->cod_comision);
			///VERIFICO QUE EL ARCHIVO NO EXISTA. SI EXISTE QUIERE DECIR QUE YA HAY UNO PARA ESA FECHA Y COMISION, DEBE ARROJAR ERROR.
			if (access(nombreAsistencia, F_OK) == -1){
				
				mostrarYGuardarMSJ("El Servidor envia.");
				FILE *nueArchAsistencia = fopen(nombreAsistencia, "wt");
				fprintf(nueArchAsistencia, "%s|%s\n" , "NOMBRE", "PRESENTE");
				///ME POSICIONO AL PRINCIPIO DEL ARCHIVO USUARIOS.TXT.
				fseek(arch, 0, SEEK_SET);
				fgets(lineas, 100, arch); //Ignoro la primera linea.
			
				fgets(lineas, 100, arch);
	
				while(!feof(arch)){
		
					token = strtok(lineas, delimitador);
					if(token !=NULL){
						strcpy(alumnos.nombre, token);	
					}
		
					token = strtok(NULL, delimitador);
					if(token !=NULL){
						strcpy(alumnos.password, token);	
					}
					
					token = strtok(NULL, delimitador);
					if(token !=NULL){
						strcpy(alumnos.rol, token);	
					}
					
					token = strtok(NULL, delimitador);
					if(token !=NULL){
						alumnos.cod_comision = atoi(token);
				
					}
					
					///VERIFICO QUE NO SEAN DEL MISMO ROL PERO QUE SEAN DE LA MISMA COMISION.
					if(strcmp(alumnos.rol, usuarioLoggeado->rol) != 0 && alumnos.cod_comision == usuarioLoggeado->cod_comision){	
						///ENVIO EL NOMBRE DE LOS ALUMNOS DE ESA COMISION
						enviarMensaje(alumnos.nombre, socketCliente, tamPaquete);
						mostrarYGuardarMSJ(alumnos.nombre);
						usleep(1 * 1000);
						///RECIBO LA ASISTENCIA.
						recv(socketCliente, query, sizeof(t_request), 0);
						mostrarYGuardarMSJ(query->linea);
						//snprintf(grabar, sizeof(grabar), "$s|%s" , alumnos.nombre, query->linea);
						///ALUMNO|P
						fprintf(nueArchAsistencia, "%s|%s\n" , alumnos.nombre, query->linea);
						fflush(stdout);
					}

					fgets(lineas, 100, arch);
					token = NULL;
				}
				fclose(nueArchAsistencia);
				const char fin[] = "FIN";
				enviarMensaje(fin, socketCliente, tamPaquete);
				usleep(1 * 1000);
			}else{
				mostrarYGuardarMSJ("El Servidor envia: Archivo EXISTENTE para esa fecha.");
				const char error[] = "ERROR";
				enviarMensaje(error, socketCliente, tamPaquete);
				usleep(1 * 1000);
			}
			
		}
	}
	else {
		///EL ALUMNO CONSULTA SI ASISTIO EN DETERMINADA FECHA.
		if(query->operacion == 1){
			mostrarYGuardarMSJ("El Cliente consulta su Asistencia.");
			snprintf(nombreAsistencia, sizeof(nombreAsistencia), "%sAsistencia_%s_%d.txt" , DIR_ASISTENCIA, query->linea, usuarioLoggeado->cod_comision);
			
			if (access(nombreAsistencia, F_OK) != -1){ //Verifico si existe el archivo;
			
				mostrarYGuardarMSJ("El Servidor envia.");
				char *token;
				int i = 0;
				t_asistencia alumno;
				char enviarAsistencia[100];
				
				FILE *archAsistencia = fopen(nombreAsistencia, "rt"); //Abro el archivo.
				fgets(lineas, 100, archAsistencia); //Leo toda la linea del archivo.
				fgets(lineas, 100, archAsistencia);
				
				while(!feof(archAsistencia) && i == 0){
					
					token = strtok(lineas, delimitador);
					if(token !=NULL){
						strcpy(alumno.nombre, token);	
					}
		
					token = strtok(NULL, delimitador);
					if(token !=NULL){
						strcpy(alumno.asistencia, token);	
					}
					
					if(strcmp(alumno.nombre, usuarioLoggeado->nombre) == 0){
						
						if(strcmp(alumno.asistencia, "P") == 0){
							
							sprintf(enviarAsistencia, "%s Usted estuvo PRESENTE el dia %s", usuarioLoggeado->nombre, query->linea);
						
						}else{
							
							sprintf(enviarAsistencia, "%s Usted estuvo AUSENTE el dia %s", usuarioLoggeado->nombre, query->linea);
						}
						i = 1;
						mostrarYGuardarMSJ(enviarAsistencia);
						enviarMensaje(enviarAsistencia, socketCliente, tamPaquete);
						usleep(1 * 1000);
					}
					
					fgets(lineas, 100, archAsistencia);
					token = NULL;
					
				}
				
				fclose(archAsistencia);
				
			}else{
				mostrarYGuardarMSJ("Servido envia: TODAVIA NO SE CREO UN ARCHIVO PARA ESA FECHA.");
				const char error[] = "ERROR";
				enviarMensaje(error, socketCliente, tamPaquete);
				
			}
		
		}
		///EL ALUMNO CONSULTA EL PORCENTAJE DE ASISTENCIA/INASISTENCIA QUE TIENE.
		if(query->operacion == 2){
			///ABRO EL DIRECTORIO.
			DIR *dir = opendir ("./Asistencias");
			struct dirent *ent;
			int comision;
			float ausente = 0;
			float presente = 0;
			
			///Miramos que no haya error 
			if (dir == NULL)
				printf("No puedo abrir el directorio\n");
			
			mostrarYGuardarMSJ("El Cliente solicita su porcentaje de ASISTENCIA/INASISTENCIA.");
			mostrarYGuardarMSJ("El Servidor envia.");
			
			///LEYENDO UNO A UNO TODOS LOS ARCHIVOS QUE HAY EN EL DIRECTORIO.
			while ((ent = readdir (dir)) != NULL){
				///Nos devolverá el directorio actual (.) y el anterior (..), como hace ls.
				if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0){
					///Una vez tenemos el archivo, lo pasamos a una funcion para procesarlo.
					comision = obtenerComision(ent->d_name);
					///VERIFICAMOS QUE SE DE LA MISMA COMISION.
					if(comision == usuarioLoggeado->cod_comision){
						///CALCULO LA CANTIDAD DE ASISTENCIA E INASISTENCIA.
						calcularPorcentaje(ent->d_name, &presente, &ausente, usuarioLoggeado);
					}
				}
			}
			
			closedir (dir);
			
			float porcentajeAsistencia = (presente / (presente + ausente)) * 100;
			float porcentajeInasistencia = (ausente / (presente + ausente)) * 100;
			
			char mensaje[100];
			sprintf(mensaje, "Porcentaje de Asistencia: %.0f%%\nPorcentaje de Inasistencia: %.0f%%", porcentajeAsistencia,  porcentajeInasistencia);
			mostrarYGuardarMSJ(mensaje);
			enviarMensaje(mensaje, socketCliente, tamPaquete);
			
		}
	}		
	
}

int obtenerComision(char *nombreArch){
	
	char *token;
	char archivo[100];
	char nombre[100];
	char fecha[100];
	int comision;
	char delimitador[] = "_.\n";
	strcpy(archivo, nombreArch); //Copio el nombre del archivo porque sino se me modifica.
	
	token = strtok(archivo, delimitador);
	if(token !=NULL){
		strcpy(nombre, token);	
	}
		
	token = strtok(NULL, delimitador);
	if(token !=NULL){
		strcpy(fecha, token);	
	}
					
	token = strtok(NULL, delimitador);
	if(token !=NULL){
		comision = atoi(token);	
	}
	token = NULL;
	
	return comision;	
}

void calcularPorcentaje(char *nomArchivo, float *presente, float *ausente, t_datos *usuarioLoggeado){
	
	t_asistencia alumno;
	char archivo[100];
	char lineas[100];
	char *token;
	char delimitador[] = "|\n";
	float p = 0;
	float a = 0;
	snprintf(archivo, sizeof(archivo), "%s%s" , DIR_ASISTENCIA, nomArchivo);
	
	FILE *file = fopen(archivo, "rt");
	fgets(lineas, 100, file);
	fgets(lineas, 100, file);
	while(!feof(file)){

		token = strtok(lineas, delimitador);
		if(token !=NULL){
			strcpy(alumno.nombre, token);	
		}
		
		token = strtok(NULL, delimitador);
		if(token !=NULL){
			strcpy(alumno.asistencia, token);	
		}
		
		if(strcmp(alumno.nombre, usuarioLoggeado->nombre) == 0){
	
			if(strcmp(alumno.asistencia, "P") == 0){
				(*presente)++;
			}else{
				(*ausente)++;
			}
			
		}
		token = NULL;
		fgets(lineas, 100, file);
	}
	
	fclose(file);
}

void mostrarYGuardarMSJ(char *msj){
	
	printf("%s\n", msj);
	log_message(LOG_FILE, msj);
	fflush(stdout);
	
}

////////////////////////////////////////////////////////////////////////////////////	

void enviarMensaje(const char *mensaje, int socketCliente, char *tamPaquete)
{
    if (strcmp(mensaje, "FIN") != 0)
    {
        sprintf(tamPaquete, "%lu", strlen(mensaje));
        send(socketCliente, tamPaquete, sizeof(int), 0);
        send(socketCliente, mensaje, strlen(mensaje), 0);
    }
    else
    {
        sprintf(tamPaquete, "%d", (int)sizeof(mensaje));
        send(socketCliente, tamPaquete, sizeof(int), 0);
        send(socketCliente, mensaje, sizeof(mensaje), 0);
    }
}

void set(const char *ip, const char *puerto)
{
    bzero(&(configuracionSocket.sin_zero), 8);
    configuracionSocket.sin_family = AF_INET;
    configuracionSocket.sin_addr.s_addr = inet_addr(ip);
    configuracionSocket.sin_port = htons(atoi(puerto));
}

int comprobacionBD(char *path)
{
    ///Primero se comprueba que el archivo de la base de datos exista.
    if (access(path, F_OK) != -1)
    {
        strcpy(bd, path);
    }
    else
    {
        printf("El archivo de la Usuarios no existe\n");
        return 1;
    }

    return 0;
}

int creacionSocket(int *serverSocket, int *habilitar)
{
    ///CREO EL SOCKET
    if ((*serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("No se pudo crear el socket!\n");
        return 1;
    }

    ///Configurar opciones del socket
    if (setsockopt(*serverSocket, SOL_SOCKET, SO_REUSEADDR, habilitar, sizeof(int)) == -1)
    {
        printf("No se pudo configurar las opciones del socket!\n");
        return 1;
    }

    return 0;
}

int bindListen(int *serverSocket)
{
    ///Vinculo el socket con la configuracion
    if (bind(*serverSocket, (struct sockaddr *)&configuracionSocket, sizeof(struct sockaddr_in)) < 0)
    {
        printf("El puerto ingresado ya esta siendo utilizado.");
        exit(1);
    }

    /// Empiezo a escuchar en el puerto especificado anteriormente
    listen(*serverSocket, MAX_QUEUE);
    char esperandoConexion[100];
    sprintf(esperandoConexion, "Esperando conexiones en el puerto %d...", ntohs(configuracionSocket.sin_port));
    printf("%s\n", esperandoConexion);
    log_message(LOG_FILE, esperandoConexion);

    fflush(stdout);
    return 0;
}

void aceptarRequests(pthread_t *tid, int *socketCliente, int *serverSocket, struct sockaddr_in *ca, socklen_t *cl)
{
    *socketCliente = accept(*serverSocket, (struct sockaddr *)ca, cl);
	
	char nuevoCliente[] = "[NUEVA CONEXION] Un cliente nuevo se conecto al servidor.";
	printf("%s\n", nuevoCliente);

    log_message(LOG_FILE, nuevoCliente);
    fflush(stdout);

    ///Tengo que crear un hilo por cada usuario
    ///De esta forma, aseguro que no se crucen
    pthread_create(tid, NULL, &obtenerQuery, socketCliente);
}
