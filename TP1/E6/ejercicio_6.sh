#! /bin/bash
#
# -----------------------ENCABEZADO------------------------------------------------------------------------#
#
# Nombre del script: tp1_ejercicioX
# Trabajo practico: 1
# Ejercicio: 6
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

 function mcd() (
     ! (($1%$2)) && return $2 || mcd $2 $(($1%$2))
 )

function suma () {
    mcd $2 $4
    res=$?
    mcm=$(( ($2*$4) / $res ))
    numRes=$(( ($1*$mcm/$2) + ($3*$mcm/$4) ))
    denRes=$mcm
}
# ------------------------FIN FUNCIONES---------------------------------------------------------------------#
#
# -------------------AYUDA Y VALIDACIONES------------------------------------------------------------------#
if [[ $1 == "-h" || $1 == "-?" || $1 == "-help" ]]
then
 echo "Este Script permite realizar la suma de todos los números fraccionarios contenidos en un archivo"
 echo "Guardando en un archivo salida.out (en el mismo directorio) el resultado e informandolo por salida standard"
 echo "Ayuda: como primer parametro debe escribir el path del archivo a analizar."
 echo "Ejemplo: ./ejercicio_6.sh  -f <path_archivo> "
 exit
fi

#Validacion de parametros 
if ! [[ $# == 2 ]]  
    then
    echo "ERROR. Cantidad de parametros invalido."
    exit
fi

if ! [[ "$1" == "-f" ]]
then
	echo "ERROR. Parametro incorrecto por favor para ver la ayuda utilizar los parametros -h -? o -help"
    exit
fi

#Validacion archivo, que exista archivo y que no vacio.
if ! [[ -f "$2" ]]
then
    echo "ERROR. Archivo inexistente"
    exit
else
    if ! [[ -s "$2" ]]
    then
        echo "Error el archivo en la ruta $2 esta vacia ."
        exit
    fi
fi
# -------------------FIN AYUDA Y VALIDACIONES---------------------------------------------------------------#
#
# ----------------------------MAIN--------------------------------------------------------------------------#

#Guardo archivo en una cadena
cadNum="$( awk '{cadNum=$1} {print cadNum;}' "$2" )"

IFS=',' read -a cadNum <<< $cadNum

VecND=""

#Convertirmos fracciones mixtas a impropia
for (( i=0; i<${#cadNum[@]}; i++ ))
do
    numero=${cadNum[$i]} 
    [[ $numero =~ ([-0-9]+)(\:)([-0-9/]+) ]]
    entero=${BASH_REMATCH[1]} #capturo el entero
    fraccion=${BASH_REMATCH[3]} #captura fraccion
    if [[ $entero == "" ]] #si no hay entero
    then
        [[ $numero =~ ([-0-9]+)/([0-9]+) ]]
        numerador=${BASH_REMATCH[1]} #separo en numerador y
        denominador=${BASH_REMATCH[2]} #denominador capturandolos
        VecND+=$numerador,$denominador, 
    
    else
      
     [[ $fraccion =~ ([-0-9]+)/([0-9]+) ]]
        numerador=${BASH_REMATCH[1]} #separo en numerador
        denominador=${BASH_REMATCH[2]} #y denominador capturandolos 

        #normalizamos el signo        
        if (( $entero*$numerador < 0 ))
        then
            signo=-1
        else
            signo=1
        fi
        if (( $entero < 0 ))
        then
            entero=$(( $entero*(-1) ))
        fi
        if (( $numerador < 0 ))
        then
            numerador=$(( $numerador*(-1) ))
        fi
        numerador=$(( ($entero*$denominador+$numerador) * $signo )) 
        VecND+=$numerador,$denominador,   
    fi
done

#Guarda cada corte en una posicion del vector
IFS=',' read -a VecND <<< $VecND
numRes=${VecND[0]}
denRes=${VecND[1]}

for (( i=2; i<${#VecND[@]}; i+=2 ))
do
    num2=${VecND[$i]}
    den2=${VecND[$i+1]}
    #hago la suma contemplando el algoritmo de mcd de euclides
    suma $numRes $denRes $num2 $den2
done


#compruebo que numerador no sea 0 
if (( $numRes == 0 ))
    then 
        echo 0
        echo 0 > "$PWD/salida.out"
    else
    echo "$numRes/$denRes"
    echo "$numRes/$denRes" > "$PWD/salida.out"
fi

# ------------------------FIN MAIN-------------------------------------------------------------------------#
#
# -----------------------FIN SCRIPT------------------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------# 