#!/bin/bash
#
# -----------------------ENCABEZADO------------------------------------------------------------------------#
#
# Nombre del script: Ejercicio3
# Trabajo practico: 1
# Ejercicio: 3
# Entrega: 2da
# Integrantes:
#	    Daiana Gomez Nespola, DNI 38005120
#	    Juan Masi, DNI 37981647
#       Guido Delbo, DNI 36982261
#	    Nicolas Fortunato, DNI 37864810
#	    Damian Perez, DNI 35375255
#
# ----------------------FIN ENCABEZADO---------------------------------------------------------------------#
#
# --------------------------SCRIPT-------------------------------------------------------------------------#

# --------------------------FUNCIONES-----------------------------------------------------------------------#

function ayuda() {
    echo "Explicacion ejercicio numero 3:"
    echo "Se crea un demonio que evalua todos los archivos logs de un directorio"
    echo "los archivos deben tener el siguiente formato nombreEmpresa-numeroSemana.log"
    echo "cada cierto intervalo y elimina los logs viejos de cada empresa"
    echo "-----------------------------------------------------------------------------"
    echo "Sintaxis ejercicio numero 3:"
    echo "  ./Source.sh -f (Path) -t (Segundos)"
    echo "  Path: Directorio a evaluar."
    echo "  Segundos: Intervalo en que debe ser evaluado el directorio enviado como parametro."
    echo "-----------------------------------------------------------------------------"
    echo "Ejemplo de ejecucion ejercicio numero 3:"
    echo "  ./ejercicio3.sh -f ./archivos -t 10"
}

# ------------------------FIN FUNCIONES---------------------------------------------------------------------#

# -------------------AYUDA Y VALIDACIONES------------------------------------------------------------------#

if [[ "$1" == "-help" || "$1" == "-h" || "$1" == "-?" ]]
then
    ayuda
    exit 1;
fi

#Cantidad de parametros del script
if [ $# != 4 ]
then
    echo "Cantidad de parametros invalida"
    exit 2;
fi

#Formato del path e intervalo
if [[ "$1" != "-f" || "$3" != "-t" ]]
then
    echo "Error en el formato de los parametros"
    exit 3;
fi

if [ $4 -lt 0 ]
then
    echo "Intervalo de tiempo ingresado negativo"
    exit 4;
fi

if ! test -e "$2" 
then
    echo "El path ingresado no existe"
    exit 5;
fi

if ! test -d "$2" 
then
    echo "El path ingresado no es un directorio valido"
    exit 6;
fi

# -------------------FIN AYUDA Y VALIDACIONES---------------------------------------------------------------#
#
# ----------------------------MAIN--------------------------------------------------------------------------#

#Inicia el demonio
#Se le pasa el directorio a analizar y el intervalo en que debera analizarlo

./demonio.sh $2 $4 &

# ------------------------FIN MAIN-------------------------------------------------------------------------#
#
# -----------------------FIN SCRIPT------------------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------#
