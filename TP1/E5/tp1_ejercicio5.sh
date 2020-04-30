#! /bin/bash
#
# -----------------------ENCABEZADO------------------------------------------------------------------------#
#
# Nombre del script: tp1_ejercicio5
# Trabajo practico: 1
# Ejercicio: 5
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

function opcionesAyuda {
    echo "Para ver la Ayuda de este script ingrese:"
    echo ""
    echo "\"$0 -h\", \"-help\" ó \"-?\"."
}

# ------------------------FIN FUNCIONES---------------------------------------------------------------------#
#
# -------------------AYUDA Y VALIDACIONES------------------------------------------------------------------#

if [ $# == 0 ] || [ $# -gt 2 ]
then
    opcionesAyuda
    exit
fi

if [ $# == 1 ]
then
    if [ "$1" == "-h" ] || [ "$1" == "-help" ] || [ "$1" == "-?" ]
    then
        echo "Este script calcula las estadísticas de aprobación y deserción de su alumnado."
        echo -e "\nEjecución:\n"
        echo "Ingrese \"$0 -f [ARCHIVO]\"        para realizar el cálculo."
        exit
    else
        echo "Parámetro no reconocido."
        opcionesAyuda
        exit
    fi
fi

if [ $# == 2 ]
then
    if [ "$1" != "-f" ] || [ ! -f "$2" ]
    then    
        echo "Parámetro/s no reconocido/s."
        opcionesAyuda
        exit
    fi
fi

# -------------------FIN AYUDA Y VALIDACIONES---------------------------------------------------------------#
#
# ----------------------------MAIN--------------------------------------------------------------------------#

gawk 'function materia_nueva(id) {
    
    for(m in materias) {
        if(materias[m] == id)
            return 0
    }

    return 1
}
function es_valido(dni, id, notaPri, notaSeg, rec, notaRec, notaFin) {
    
    if(dni !~ /^[0-9]{1,8}$/) {
        return 0
    }
    
    if(id !~ /^[0-9]*$/) {
        return 0
    }
    

    if(notaFin ~ /^[4-9]$|^10$/) {
        return 0
    }

    if( (notaPri ~ /^[7-9]$|^10$/) && (notaSeg ~ /^[7-9]$|^10$/) ) {
        return 0
    }

    if( (notaPri ~ /^[7-9]$|^10$/) && (notaSeg ~ /^[0-6]$|^\s*$/) && (rec == "2") && (notaRec ~ /^[7-9]$|^10$/) ) {
        return 0
    }

    if( (notaSeg ~ /^[7-9]$|^10$/) && (notaPri ~ /^[0-6]$|^\s*$/) && (rec == "1") && (notaRec ~ /^[7-9]$|^10$/) ) {
        return 0
    }
    
    return 1
}
function abandona(notaPri, notaSeg, notaRec) {

    if( (notaPri ~ /^[0-3]$|^\s*$/) && (notaSeg ~ /^\s*$/) ) {
        return 1
    }

    if( (notaSeg ~ /^[0-3]$/) && (notaPri ~ /^\s*$/) ) {
        return 1
    }

    if( (notaPri ~ /^[4-9]$|^10$/) && (notaSeg ~ /^\s*$/) && (notaRec ~ /^\s*$/) ) {
        return 1
    }

    if( (notaSeg ~ /^[4-9]$|^10$/) && (notaPri ~ /^\s*$/) && (notaRec ~ /^\s*$/) ) {
        return 1
    }

    return 0
}
function recupera(notaPri, notaSeg, rec, notaRec) {

    if( notaRec != "" ) {
        return 0
    }

    if( (notaPri ~ /^[0-3]$/) && (notaSeg ~ /^[4-9]$|^10$/) && (rec ~ /^1$|^\s*$/) && (notaRec ~ /^\s*$/) ) {
        return 1
    }

    if( (notaSeg ~ /^[0-3]$/) && (notaPri ~ /^[4-9]$|^10$/) && (rec ~ /^2$|^\s*$/) && (notaRec ~ /^\s*$/) ) {
        return 1
    }

    if( (notaPri ~ /^[4-6]$/) && (notaSeg ~ /^[7-9]$|^10$/) && (rec ~ /^1$|^\s*$/) && (notaRec ~ /^\s*$/) ) {
        return 1
    }

    if( (notaSeg ~ /^[4-6]$/) && (notaPri ~ /^[7-9]$|^10$/) && (rec ~ /^2$|^\s*$/) && (notaRec ~ /^\s*$/) ) {
        return 1
    }

    return 0
}
function final(notaPri, notaSeg, rec, notaRec, notaFin) {

    if( notaFin != "" ) {
        return 0
    }

    if( (notaPri ~ /^[4-6]$/) && (notaSeg ~ /^[4-6]$/) ) {
        return 1
    }

    if( (notaPri ~ /^[4-6]$/) && (notaSeg ~ /^[0-3]$|^\s*$/) && (rec == 2) && (notaRec ~ /^[4-9]$|^10$/) ) {
        return 1
    }

    if( (notaSeg ~ /^[4-6]$/) && (notaPri ~ /^[0-3]$|^\s*$/) && (rec == 1) && (notaRec ~ /^[4-9]$|^10$/) ) {
        return 1
    }

    if( (notaPri ~ /^[7-9]$|^10$/) && (notaSeg ~ /^[0-6]$|^\s*$/) && (rec == 2) && (notaRec ~ /^[4-6]$/) ) {
        return 1
    }

    if( (notaSeg ~ /^[7-9]$|^10$/) && (notaPri ~ /^[0-6]$|^\s*$/) && (rec == 1) && (notaRec ~ /^[4-6]$/) ) {
        return 1
    }

    return 0
}
function recursa(notaPri, notaSeg, rec, notaRec, notaFin) {

    if( (notaPri ~ /^[0-3]$/) && (notaSeg ~ /^[0-3]$/) ) {
        return 1
    }

    if( (notaPri ~ /^[4-9]$|^10$/) && (notaSeg ~ /^[4-6]$/) && (notaFin ~ /^[0-3]$/) ) {
        return 1
    }

    if( (notaSeg ~ /^[4-9]$|^10$/) && (notaPri ~ /^[4-6]$/) && (notaFin ~ /^[0-3]$/) ) {
        return 1
    }

    if( (notaPri ~ /^[4-6]$/) && (notaSeg ~ /^[0-3]$|^\s*$/) && (rec == 2) && (notaRec ~ /^[4-9]$|^10$/) && (notaFin ~ /^[0-3]$/) ) {
        return 1
    }

    if( (notaSeg ~ /^[4-6]$/) && (notaPri ~ /^[0-3]$|^\s*$/) && (rec == 1) && (notaRec ~ /^[4-9]$|^10$/) && (notaFin ~ /^[0-3]$/) ) {
        return 1
    }

    if( (notaPri ~ /^[4-9]$|^10$/) && (notaSeg ~ /^[0-3]$|^\s*$/) && (rec == 2) && (notaRec ~ /^[0-3]$/) ) {
        return 1
    }

    if( (notaSeg ~ /^[4-9]$|^10$/) && (notaPri ~ /^[0-3]$|^\s*$/) && (rec == 1) && (notaRec ~ /^[0-3]$/) ) {
        return 1
    }

    if( (notaPri ~ /^[7-9]$|^10$/) && (notaSeg ~ /^[0-6]$|^\s*$/) && (rec == 2) && (notaRec ~ /^[4-6]$/) && (notaFin ~ /^[0-3]$/) ) {
        return 1
    }

    if( (notaSeg ~ /^[7-9]$|^10$/) && (notaPri ~ /^[0-6]$|^\s*$/) && (rec == 1) && (notaRec ~ /^[4-6]$/) && (notaFin ~ /^[0-3]$/) ) {
        return 1
    }

    if( (notaPri ~ /^[7-9]$|^10$/) && (notaSeg ~ /^[4-6]$/) && (rec == 2) && (notaRec ~ /^[0-3]$/) ) {
        return 1
    }

    if( (notaSeg ~ /^[7-9]$|^10$/) && (notaPri ~ /^[4-6]$/) && (rec == 1) && (notaRec ~ /^[0-3]$/) ) {
        return 1
    }


    return 0
}
BEGIN {FS = "|"; i=0; materias[0]=0;}
(es_valido($1, $2, $3, $4, $5, $6, $7) == 1) {
    
    if(materia_nueva($2) == 1)
        materias[i]=$2
    
    if(abandona($3, $4, $6) == 1)
        aban[$2]++

    if(recupera($3, $4, $5, $6) == 1)
        recup[$2]++

    if(final($3, $4, $5, $6, $7) == 1)
        fin[$2]++

    if(recursa($3, $4, $5, $6, $7) == 1)
        recur[$2]++

    i++
} 
END {
    if(i>0) {
        print "Materia, Final, Recursan, Recuperan, Abandonaron"

        for(x in materias) {
            id=materias[x]
            printf("      %d,     %d,        %d,         %d,           %d\n", id, fin[id], recur[id], recup[id], aban[id])
        }

    }
}' "$2" > ./salida.txt

# ------------------------FIN MAIN-------------------------------------------------------------------------#
#
# -----------------------FIN SCRIPT------------------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------#