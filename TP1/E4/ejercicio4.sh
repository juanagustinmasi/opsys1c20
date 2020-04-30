#!/bin/bash
#
# -----------------------ENCABEZADO------------------------------------------------------------------------#
#
# Nombre del script: tp1_ejercicioX
# Trabajo practico: 1
# Ejercicio: X
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



# ------------------------FIN FUNCIONES---------------------------------------------------------------------#
#
# -------------------AYUDA Y VALIDACIONES------------------------------------------------------------------#

if ! [[ $# -eq 4 || $# -eq 6 ]]
then
	echo "La cantidad de parametros ingresados es incorrecta"
   	exit 1;
fi

if ! [[ $# == 4 && "$1" == "-f" && "$3" == "-z" ]] && ! [[ $# == 6 && "$1" == "-f" && "$3" == "-z" && $5 == "-e" ]]
then
	echo "El o los parametros no son correctas."
    	exit 2;
fi

if ! test -d "$2" 
then
	echo "El path donde se encuentran los archivos de log no es un directorio valido."
	exit 3;
fi

if ! test -d "$4" 
then
	echo "El path donde se van a comprimir los archivos de log no es un directorio valido."
	exit 4;
fi

# -------------------FIN AYUDA Y VALIDACIONES---------------------------------------------------------------#
#
# ----------------------------MAIN--------------------------------------------------------------------------#

if test $# -eq 4
then	
	origen=`pwd` 
	origen="$origen/$2" 
	destino=`pwd` 
	destino="$destino/$4/archivo.zip" 
	cd "$origen";

	EMPRESA_INICIAL=" "       
	
	for file in `find "$origen" -maxdepth 1 -type f -regex ".+-[0-9]+.log" -exec basename {} \; | sort -r -V`
		do
        		if [[ $(echo "$file" | cut -d '-' -f 1) = $(echo "$EMPRESA_INICIAL" | cut -d '-' -f 1) ]]
       			then
            			echo "$file"
	    			zip -r "$destino" "$file"
            			rm "$origen/$file"
      		  	else
            			EMPRESA_INICIAL=$file
       			fi
   	 done
else

	origen=`pwd` 
	origen="$origen/$2" 
	destino=`pwd` 
	destino="$destino/$4/archivo.zip" 
	cd "$origen";
	
	VERSION=1
		
	for file in `find "$origen" -maxdepth 1 -type f -regex ".+-[0-9]+.log" -exec basename {} \; | sort -r -V`
    		do
	        	if [[ $(echo "$6" | cut -d '-' -f 1) = $(echo "$file" | cut -d '-' -f 1) ]]
	       		then
				if [[ $VERSION -ne 0 ]]
		                then
		                    VERSION=0
               			else
		            		echo "$file"
			    		zip -r "$destino" "$file"
            				rm "$origen/$file"
        			fi
			fi
    		done	
fi

# ------------------------FIN MAIN-------------------------------------------------------------------------#
#
# -----------------------FIN SCRIPT------------------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------#