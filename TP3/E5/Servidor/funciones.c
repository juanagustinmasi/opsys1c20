/*-----------------------ENCABEZADO------------------------------------------------------------------------

 Nombre del programa: funciones.c
 Trabajo practico: 3
 Ejercicio: 5
 Entrega: 3ra
 Integrantes:
	    Daiana Gomez Nespola, DNI 38005120
	    Juan Masi, DNI 37981647
        Guido Delbo, DNI 36982261
	    Nicolas Fortunato, DNI 37864810
	    Damian Perez, DNI 35375255

 ----------------------FIN ENCABEZADO---------------------------------------------------------------------*/


#include "funciones.h"

void terminarProceso()
{
	grabarLog("./servidor.log", "Servidor cerrado, todos los procesos terminaron correctamente.");
	exit(0);
}

void grabarLog(char *nombreArchivo, char *msg)
{
	FILE *pf;
	pf = fopen(nombreArchivo, "a");
	if (!pf)
		return;
	fprintf(pf, "%s\n", msg);
	fclose(pf);
}

void recibeConexion(pthread_t *thread, int *socketCliente, int *socketServidor, struct sockaddr_in *addr, socklen_t *addrlen)
{
	char logClienteConectado[200];

	*socketCliente = accept(*socketServidor, (struct sockaddr *)addr, addrlen);
	pthread_create(thread, NULL, &recibeConsulta, socketCliente);

	sprintf(logClienteConectado, "Cliente nro. \"%ld\" CONECTADO.", *thread);
	printf("\n%s\n", logClienteConectado);
	grabarLog("./servidor.log", logClienteConectado);
	fflush(stdout);
}

int loguea(FILE *pf, int socketCliente, tUsuario *info, tInfo *registro)
{
	char linea[100];
	char delimitador[] = "|\n";
	char *token;
	char docente[50];
	char alumno[50];
	char tamMsg[10];

	fseek(pf, 0, SEEK_SET);
	fgets(linea, 100, pf);
	fgets(linea, 100, pf);

	while (!feof(pf))
	{
		token = strtok(linea, delimitador);

		if (token != NULL)
		{
			strcpy(registro->nombre, token);
		}

		token = strtok(NULL, delimitador);

		if (token != NULL)
		{
			strcpy(registro->clave, token);
		}

		token = strtok(NULL, delimitador);

		if (token != NULL)
		{
			strcpy(registro->rol, token);
		}

		token = strtok(NULL, delimitador);

		if (token != NULL)
		{
			registro->cod_comision = atoi(token);
		}

		if (strcmp(registro->nombre, info->usuario) == 0 && strcmp(registro->clave, info->clave) == 0)
		{

			if (strcmp(registro->rol, "D") == 0)
			{
				sprintf(docente, "DOCENTE=%d", registro->cod_comision);
				enviaAlSocket(docente, socketCliente, tamMsg);
				usleep(1 * 1000);
			}
			else
			{
				sprintf(alumno, "ALUMNO=%d", registro->cod_comision);
				enviaAlSocket(alumno, socketCliente, tamMsg);
				usleep(1 * 1000);
			}

			char loggeado[100];
			sprintf(loggeado, "El usuario %s ha iniciado sesion.", info->usuario);
			printf("%s\n", loggeado);
			grabarLog("./servidor.log", loggeado);
			fflush(stdout);

			return 1;
		}

		fgets(linea, 100, pf);
		token = NULL;
	}
	enviaAlSocket(ERROR, socketCliente, tamMsg);
	usleep(1 * 1000);
	return 0;
}

void procesaConsulta(FILE *pf, int socketCliente, tInfoSocket *info, tInfo *usuarioActual)
{
	char lineas[100];
	char delimitador[] = "|\n";
	char tamMsg[10];
	char nombreAsistencia[500];

	if (strcmp(usuarioActual->rol, "D") == 0)
	{
		if (info->codigo == 1)
		{
			imprimirMsg("DOCENTE -> Listado de Asistencias");
			snprintf(nombreAsistencia, sizeof(nombreAsistencia), "%sAsistencia_%s_%d.txt", "./Asistencia_Fecha_Comision/", info->dato, usuarioActual->cod_comision);

			if (access(nombreAsistencia, F_OK) != -1)
			{
				imprimirMsg("Listado de Asistencias enviado");
				FILE *archAsistencia = fopen(nombreAsistencia, "rt");
				fgets(lineas, 100, archAsistencia);

				while (!feof(archAsistencia))
				{
					enviaAlSocket(lineas, socketCliente, tamMsg);
					imprimirMsg(lineas);
					usleep(1 * 1000);
					fgets(lineas, 100, archAsistencia);
				}

				fclose(archAsistencia);
				enviaAlSocket(FIN, socketCliente, tamMsg);
			}
			else
			{
				imprimirMsg("Error, no se encuentra el archivo");
				enviaAlSocket(ERROR, socketCliente, tamMsg);
			}
		}

		if (info->codigo == 2)
		{
			char *token;
			tInfo alumnos;
			time_t tiempo = time(NULL);
			struct tm *tiempoLocal = localtime(&tiempo);
			char fecha[20];

			imprimirMsg("DOCENTE -> Cargar Asistencias");
			strftime(fecha, sizeof(fecha), "%Y-%m-%d", tiempoLocal);
			snprintf(nombreAsistencia, sizeof(nombreAsistencia), "%sAsistencia_%s_%d.txt", "./Asistencia_Fecha_Comision/", fecha, usuarioActual->cod_comision);
			
			if (access(nombreAsistencia, F_OK) == -1)
			{
				FILE *nueArchAsistencia = fopen(nombreAsistencia, "wt");

				imprimirMsg("Carga de Asistencias iniciada");
				fprintf(nueArchAsistencia, "%s|%s\n", "NOMBRE", "ASISTENCIA");
				fseek(pf, 0, SEEK_SET);
				fgets(lineas, 100, pf);
				fgets(lineas, 100, pf);

				while (!feof(pf))
				{
					token = strtok(lineas, delimitador);

					if (token != NULL)
					{
						strcpy(alumnos.nombre, token);
					}

					token = strtok(NULL, delimitador);

					if (token != NULL)
					{
						strcpy(alumnos.clave, token);
					}

					token = strtok(NULL, delimitador);

					if (token != NULL)
					{
						strcpy(alumnos.rol, token);
					}

					token = strtok(NULL, delimitador);

					if (token != NULL)
					{
						alumnos.cod_comision = atoi(token);
					}

					if (strcmp(alumnos.rol, usuarioActual->rol) != 0 && alumnos.cod_comision == usuarioActual->cod_comision)
					{
						enviaAlSocket(alumnos.nombre, socketCliente, tamMsg);
						imprimirMsg(alumnos.nombre);
						usleep(1 * 1000);
						recv(socketCliente, info, sizeof(tInfoSocket), 0);
						imprimirMsg(info->dato);
						fprintf(nueArchAsistencia, "%s|%s\n", alumnos.nombre, info->dato);
						fflush(stdout);
					}

					fgets(lineas, 100, pf);
					token = NULL;
				}
				fclose(nueArchAsistencia);
				enviaAlSocket(FIN, socketCliente, tamMsg);
				usleep(1 * 1000);
			}
			else
			{
				imprimirMsg("Error, asistencias ya cargadas.");
				enviaAlSocket(ERROR, socketCliente, tamMsg);
				usleep(1 * 1000);
			}
		}
	}
	else
	{
		if (info->codigo == 1)
		{
			imprimirMsg("ALUMNO -> Consulta de Asistencia");
			snprintf(nombreAsistencia, sizeof(nombreAsistencia), "%sAsistencia_%s_%d.txt", "./Asistencia_Fecha_Comision/", info->dato, usuarioActual->cod_comision);

			if (access(nombreAsistencia, F_OK) != -1)
			{ 
				imprimirMsg("Consulta de Asistencia enviada");
				char *token;
				int i = 0;
				tAsistencia alumno;
				char enviarAsistencia[500];

				FILE *archAsistencia = fopen(nombreAsistencia, "rt"); 
				fgets(lineas, 100, archAsistencia);					
				fgets(lineas, 100, archAsistencia);

				while (!feof(archAsistencia) && i == 0)
				{
					token = strtok(lineas, delimitador);

					if (token != NULL)
					{
						strcpy(alumno.nombre, token);
					}

					token = strtok(NULL, delimitador);

					if (token != NULL)
					{
						strcpy(alumno.asistencia, token);
					}

					if (strcmp(alumno.nombre, usuarioActual->nombre) == 0)
					{

						if (strcmp(alumno.asistencia, "P") == 0)
						{
							sprintf(enviarAsistencia, "%s: PRESENTE.", info->dato);
						}
						else
						{
							sprintf(enviarAsistencia, "%s : AUSENTE.", info->dato);
						}

						i = 1;
						imprimirMsg(enviarAsistencia);
						enviaAlSocket(enviarAsistencia, socketCliente, tamMsg);
						usleep(1 * 1000);
					}

					fgets(lineas, 100, archAsistencia);
					token = NULL;
				}

				fclose(archAsistencia);
			}
			else
			{
				imprimirMsg("Error, no hay datos disponibles para la fecha ingresada");
				enviaAlSocket(ERROR, socketCliente, tamMsg);
			}
		}

		if (info->codigo == 2)
		{
			DIR *dir = opendir("./Asistencia_Fecha_Comision/");
			struct dirent *ent;
			int comision;
			float ausente = 0;
			float presente = 0;

			if (dir == NULL)
				printf("Error leyendo el archivo de asistencias\n");

			imprimirMsg("ALUMNO -> Porcentaje de Asistencias");
			imprimirMsg("Porcentaje de Asistencias enviado");

			while ((ent = readdir(dir)) != NULL)
			{
				if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
				{
					comision = parseaComision(ent->d_name);
					if (comision == usuarioActual->cod_comision)
					{
						calcularPorcentaje(ent->d_name, &presente, &ausente, usuarioActual);
					}
				}
			}

			closedir(dir);

			float porcentajeAsistencia = (presente / (presente + ausente)) * 100;
			float porcentajeInasistencia = (ausente / (presente + ausente)) * 100;

			char msgPorcentaje[100];
			sprintf(msgPorcentaje, "Presente: %.0f%%\nAusente: %.0f%%", porcentajeAsistencia, porcentajeInasistencia);
			imprimirMsg(msgPorcentaje);
			enviaAlSocket(msgPorcentaje, socketCliente, tamMsg);
		}
	}
}

void *recibeConsulta(void *socket)
{
	int sockfd = *((int *)socket);
	FILE *pf = fopen(archivoUsuarios, "rt");
	tUsuario usuario;
	tInfo usuarioActual;
	tInfoSocket info;
	int logueoOk = 0;

	recv(sockfd, &usuario, sizeof(tUsuario), 0);
	logueoOk = loguea(pf, sockfd, &usuario, &usuarioActual);

	while (logueoOk == 0)
	{ 
		recv(sockfd, &usuario, sizeof(tUsuario), 0);
		logueoOk = loguea(pf, sockfd, &usuario, &usuarioActual);
	}

	recv(sockfd, &info, sizeof(tInfoSocket), 0);

	while (info.codigo != 3)
	{
		procesaConsulta(pf, sockfd, &info, &usuarioActual);
		recv(sockfd, &info, sizeof(tInfoSocket), 0);
	}

	char msjDesconexion[500];
	sprintf(msjDesconexion, "Cliente desconectado.\n\"%s\" cerró sesión.\n", usuarioActual.nombre);
	printf("%s", msjDesconexion);
	grabarLog("./servidor.log", msjDesconexion);
	fflush(stdout);
	fclose(pf);
	close(sockfd);
}

int existeArchivoUsuarios(char *path)
{
	if (access(path, F_OK) != -1)
	{
		strcpy(archivoUsuarios, path);
	}
	else
	{
		printf("Archivo de usuarios inexistente.\nSaliendo...\n\n");
		return 1;
	}

	return 0;
}

int parseaComision(char *nombreArchivo)
{
	char *token;
	char archivo[100];
	char nombre[100];
	char fecha[100];
	int comision;
	char delimitador[] = "_.\n";

	strcpy(archivo, nombreArchivo); 

	token = strtok(archivo, delimitador);

	if (token != NULL)
	{
		strcpy(nombre, token);
	}

	token = strtok(NULL, delimitador);

	if (token != NULL)
	{
		strcpy(fecha, token);
	}

	token = strtok(NULL, delimitador);

	if (token != NULL)
	{
		comision = atoi(token);
	}
	token = NULL;

	return comision;
}

int ipConfig(char *ip)
{
	struct ifaddrs *iflist, *iface;

	if (getifaddrs(&iflist) < 0)
	{
		perror("getifaddrs");
		return 1;
	}

	for (iface = iflist; iface; iface = iface->ifa_next)
	{
		int af = iface->ifa_addr->sa_family;
		const void *addr;
		char addrp[INET6_ADDRSTRLEN];

		switch (af)
		{
		case AF_INET:
			addr = &((struct sockaddr_in *)iface->ifa_addr)->sin_addr;
			break;
		case AF_INET6:
			addr = &((struct sockaddr_in6 *)iface->ifa_addr)->sin6_addr;
			break;
		default:
			addr = NULL;
		}

		if (addr)
		{
			if (inet_ntop(af, addr, addrp, sizeof addrp) == NULL)
			{
				perror("inet_ntop");
				continue;
			}
			if (!strcmp(iface->ifa_name, "lo") == 0)
			{
				strcpy(ip, addrp);
				return 0;
			}
		}
	}

	freeifaddrs(iflist);
	return 1;
}

void calcularPorcentaje(char *nombreArchivo, float *presente, float *ausente, tInfo *usuarioActual)
{
	tAsistencia alumno;
	char archivo[100];
	char lineas[100];
	char *token;
	char delimitador[] = "|\n";
	float p = 0;
	float a = 0;

	snprintf(archivo, sizeof(archivo), "%s%s", "./Asistencia_Fecha_Comision/", nombreArchivo);

	FILE *file = fopen(archivo, "rt");
	fgets(lineas, 100, file);
	fgets(lineas, 100, file);

	while (!feof(file))
	{

		token = strtok(lineas, delimitador);

		if (token != NULL)
		{
			strcpy(alumno.nombre, token);
		}

		token = strtok(NULL, delimitador);

		if (token != NULL)
		{
			strcpy(alumno.asistencia, token);
		}

		if (strcmp(alumno.nombre, usuarioActual->nombre) == 0)
		{

			if (strcmp(alumno.asistencia, "P") == 0)
			{
				(*presente)++;
			}
			else
			{
				(*ausente)++;
			}
		}

		token = NULL;
		fgets(lineas, 100, file);
	}

	fclose(file);
}

void imprimirMsg(char *msg)
{
	printf("%s\n", msg);
	grabarLog("./servidor.log", msg);
	fflush(stdout);
}

void enviaAlSocket(const char *msg, int socketCliente, char *tamMsg)
{
	if (strcmp(msg, FIN) != 0)
	{
		sprintf(tamMsg, "%lu", strlen(msg));
		send(socketCliente, tamMsg, sizeof(int), 0);
		send(socketCliente, msg, strlen(msg), 0);
	}
	else
	{
		sprintf(tamMsg, "%d", (int)sizeof(msg));
		send(socketCliente, tamMsg, sizeof(int), 0);
		send(socketCliente, msg, sizeof(msg), 0);
	}
}

/*
# ------------------------------------FIN -----------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------# 
*/