#!/bin/bash
#
# -----------------------ENCABEZADO------------------------------------------------------------------------#
#
# Nombre del script: demonio
# Trabajo practico: 1
# Ejercicio: 3
# Entrega: 1ra entrega
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

function buscarEliminar() {

    EMPRESA_INICIAL="-"

    #Se busca de forma no recursiva en el directorio los ficheros que cumplan con el formato adecuado
    #se los ordena y compara por el identificador de empresa para saber que corresponden a la misma
    for file in `find "$origen" -maxdepth 1 -type f -regex ".+-[0-9]+.log" -exec basename {} \; | sort -r -V`
    	do
      	  if [[ $(echo "$EMPRESA_INICIAL" | cut -d '-' -f 1) == $(echo "$file" | cut -d '-' -f 1) ]]
     	   then
        	    echo "$file"
          	    rm $file
     	   else
      	      EMPRESA_INICIAL=$file
      	  fi
        done
}

# ------------------------FIN FUNCIONES---------------------------------------------------------------------#
#
# ----------------------------MAIN--------------------------------------------------------------------------#

#Muestro el pid en caso de que se quiera matar al demonio
echo "Se inicia el demonio, su PID es $$"
#Me paro en el directorio a analizar
origen=`pwd`
origen="$origen"/$1
cd "$origen"
#Inicio el ciclo infinito
while [ 1 -eq 1 ]
do
    buscarEliminar
    sleep $2
done


# ------------------------FIN MAIN-------------------------------------------------------------------------#
#
# -----------------------FIN SCRIPT------------------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------#
