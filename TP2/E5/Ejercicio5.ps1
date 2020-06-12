# ----------------------------------------ENCABEZADO--------------------------------------------------------------------#
#
# Nombre del script: Ejercicio5
# Trabajo practico: 2
# Ejercicio: 5
# Entrega: 4ta
# Integrantes:
#	    Daiana Gomez Nespola, DNI 38005120
#	    Juan Masi, DNI 37981647
#       Guido Delbo, DNI 36982261
#	    Nicolas Fortunato, DNI 37864810
#	    Damian Perez, DNI 35375255
#
# ---------------------------------------FIN ENCABEZADO-----------------------------------------------------------------#
#
# -------------------------------------------SCRIPT---------------------------------------------------------------------#



# ------------------------------------AYUDA Y VALIDACIONES--------------------------------------------------------------#

<#
.SYNOPSIS
Este script calcula las estadísticas de aprobación y deserción de su alumnado.
.DESCRIPTION 
Ejecución:

./Ejercicio5.ps1 [ARCHIVO]        para realizar el cálculo.
#>


Param([Parameter(Mandatory = $true)][string]$Nomina)

$esArchivo = Test-Path $Nomina

if ($esArchivo -eq $false) {

    Write-Host "El archivo de entrada no existe."
    exit
}

# Guardo cada campo de cada línea del archivo de entrada

$registro = Import-CSV -Delimiter '|' -Path $Nomina


if ($registro.length -eq 0) {

    Write-Host "El archivo de entrada está vacío."
    exit
}


# ----------------------------------FIN AYUDA Y VALIDACIONES-----------------------------------------------------------#
#
# ------------------------------------------FUNCIONES------------------------------------------------------------------#

# Función para desestimar todos los registros que no cumplan ninguna de las 4 condiciones a contabilizar

function esValido {
    param (
        [string]$dni,
        [string]$id,
        [string]$notaPri,
        [string]$notaSeg,
        [string]$rec,
        [string]$notaRec,
        [string]$notaFin
    )

    if ($dni -notmatch '^[0-9]{1,8}$') {
        return 0
    }
    
    if ($id -notmatch '^[0-9]*$') {
        return 0
    }
    
    if ($notaFin -match '^[4-9]$|^10$') {
        return 0
    }

    if ( ($notaPri -match '^[7-9]$|^10$') -and ($notaSeg -match '^[7-9]$|^10$') ) {
        return 0
    }

    if ( ($notaPri -match '^[7-9]$|^10$') -and ($notaSeg -match '^[0-6]$|^\s*$') -and ($rec -eq "2") -and ($notaRec -match '^[7-9]$|^10$') ) {
        return 0
    }

    if ( ($notaSeg -match '^[7-9]$|^10$') -and ($notaPri -match '^[0-6]$|^\s*$') -and ($rec -eq "1") -and ($notaRec -match '^[7-9]$|^10$') ) {
        return 0
    }


    return 1
}

# Función para verificar la condición "Abandona"

function abandona {
    param (
        [string]$notaPri,
        [string]$notaSeg,
        [string]$notaRec
    )

    if ( ($notaPri -match '^[0-3]$|^\s*$') -and ($notaSeg -match '^\s*$') ) {
        return 1
    }

    if ( ($notaSeg -match '^[0-3]$') -and ($notaPri -match '^\s*$') ) {
        return 1
    }

    if ( ($notaPri -match '^[4-9]$|^10$') -and ($notaSeg -match '^\s*$') -and ($notaRec -match '^\s*$') ) {
        return 1
    }

    if ( ($notaSeg -match '^[4-9]$|^10$') -and ($notaPri -match '^\s*$') -and ($notaRec -match '^\s*$') ) {
        return 1
    }

    return 0

}

# Función para verificar la condición "Recupera"

function recupera {
    param (

        [string]$notaPri,
        [string]$notaSeg,
        [string]$rec,
        [string]$notaRec
    )

    if ( $notaRec -ne "" ) {
        return 0
    }

    if ( ($notaPri -match '^[0-3]$') -and ($notaSeg -match '^[4-9]$|^10$') -and ($rec -match '^1$|^\s*$') -and ($notaRec -match '^\s*$') ) {
        return 1
    }

    if ( ($notaSeg -match '^[0-3]$') -and ($notaPri -match '^[4-9]$|^10$') -and ($rec -match '^2$|^\s*$') -and ($notaRec -match '^\s*$') ) {
        return 1
    }

    if ( ($notaPri -match '^[4-6]$') -and ($notaSeg -match '^[7-9]$|^10$') -and ($rec -match '^1$|^\s*$') -and ($notaRec -match '^\s*$') ) {
        return 1
    }

    if ( ($notaSeg -match '^[4-6]$') -and ($notaPri -match '^[7-9]$|^10$') -and ($rec -match '^2$|^\s*$') -and ($notaRec -match '^\s*$') ) {
        return 1
    }

    return 0

}

# Función para verificar la condición "Final"

function final {
    param (
        [string]$notaPri,
        [string]$notaSeg,
        [string]$rec,
        [string]$notaRec,
        [string]$notaFin
    )

    if ( $notaFin -ne "" ) {
        return 0
    }

    if ( ($notaPri -match '^[4-6]$') -and ($notaSeg -match '^[4-6]$') ) {
        return 1
    }

    if ( ($notaPri -match '^[4-6]$') -and ($notaSeg -match '^[0-3]$|^\s*$') -and ($rec -eq 2) -and ($notaRec -match '^[4-9]$|^10$') ) {
        return 1
    }

    if ( ($notaSeg -match '^[4-6]$') -and ($notaPri -match '^[0-3]$|^\s*$') -and ($rec -eq 1) -and ($notaRec -match '^[4-9]$|^10$') ) {
        return 1
    }

    if ( ($notaPri -match '^[7-9]$|^10$') -and ($notaSeg -match '^[0-6]$|^\s*$') -and ($rec -eq 2) -and ($notaRec -match '^[4-6]$') ) {
        return 1
    }

    if ( ($notaSeg -match '^[7-9]$|^10$') -and ($notaPri -match '^[0-6]$|^\s*$') -and ($rec -eq 1) -and ($notaRec -match '^[4-6]$') ) {
        return 1
    }

    return 0

}

# Función para verificar la condición "Recursa"

function recursa {
    param (
        [string]$notaPri,
        [string]$notaSeg,
        [string]$rec,
        [string]$notaRec,
        [string]$notaFin
    )

    if ( ($notaPri -match '^[0-3]$') -and ($notaSeg -match '^[0-3]$') ) {
        return 1
    }

    if ( ($notaPri -match '^[4-9]$|^10$') -and ($notaSeg -match '^[4-6]$') -and ($notaFin -match '^[0-3]$') ) {
        return 1
    }

    if ( ($notaSeg -match '^[4-9]$|^10$') -and ($notaPri -match '^[4-6]$') -and ($notaFin -match '^[0-3]$') ) {
        return 1
    }

    if ( ($notaPri -match '^[4-6]$') -and ($notaSeg -match '^[0-3]$|^\s*$') -and ($rec -eq 2) -and ($notaRec -match '^[4-9]$|^10$') -and ($notaFin -match '^[0-3]$') ) {
        return 1
    }

    if ( ($notaSeg -match '^[4-6]$') -and ($notaPri -match '^[0-3]$|^\s*$') -and ($rec -eq 1) -and ($notaRec -match '^[4-9]$|^10$') -and ($notaFin -match '^[0-3]$') ) {
        return 1
    }

    if ( ($notaPri -match '^[4-9]$|^10$') -and ($notaSeg -match '^[0-3]$|^\s*$') -and ($rec -eq 2) -and ($notaRec -match '^[0-3]$') ) {
        return 1
    }

    if ( ($notaSeg -match '^[4-9]$|^10$') -and ($notaPri -match '^[0-3]$|^\s*$') -and ($rec -eq 1) -and ($notaRec -match '^[0-3]$') ) {
        return 1
    }

    if ( ($notaPri -match '^[7-9]$|^10$') -and ($notaSeg -match '^[0-6]$|^\s*$') -and ($rec -eq 2) -and ($notaRec -match '^[4-6]$') -and ($notaFin -match '^[0-3]$') ) {
        return 1
    }

    if ( ($notaSeg -match '^[7-9]$|^10$') -and ($notaPri -match '^[0-6]$|^\s*$') -and ($rec -eq 1) -and ($notaRec -match '^[4-6]$') -and ($notaFin -match '^[0-3]$') ) {
        return 1
    }

    if ( ($notaPri -match '^[7-9]$|^10$') -and ($notaSeg -match '^[4-6]$') -and ($rec -eq 2) -and ($notaRec -match '^[0-3]$') ) {
        return 1
    }

    if ( ($notaSeg -match '^[7-9]$|^10$') -and ($notaPri -match '^[4-6]$') -and ($rec -eq 1) -and ($notaRec -match '^[0-3]$') ) {
        return 1
    }


    return 0

}

# Clase para almacenar los registros de salida

class Materia {
    [int]$Materia
    [int]$Final
    [int]$Recursan
    [int]$Recuperan
    [int]$Abandonaron
}

# ------------------------------FIN FUNCIONES---------------------------------------------------------------#
#
# ----------------------------------MAIN--------------------------------------------------------------------#

# Declaro un array para almacenar todos los objetos de salida

$salida = @()

foreach ($reg in $registro) {

    $id = [int]$reg.IdMateria

    if (esValido $reg.DNI $id $reg.PrimerParcial $reg.SegundoParcial $reg.RecuParcial $reg.RecuNota $reg.Final) {

        $materia = New-Object -Typename Materia
        $materia.Materia = $id

        # Flag para saber si la materia actual ya la tengo en el array de salida

        $esNuevo = 1

        # Verifico la condición correspondiente e incremento el campo correspondiente

        if (final $reg.PrimerParcial $reg.SegundoParcial $reg.RecuParcial $reg.RecuNota $reg.Final) {

            # Si ya tengo la materia en el array de salida, solo incremento el campo correspondiente a cada condición

            foreach ($item in $salida) {
                if ($item.Materia -eq $id) {
                    $item.Final++
                    $esNuevo = 0
                    break
                }
            }

            # Si no tengo la materia en el array de salida, la agrego

            if ($esNuevo) {
                $materia.Final++
                $salida += $materia
            }

            continue
        }

        if (recursa $reg.PrimerParcial $reg.SegundoParcial $reg.RecuParcial $reg.RecuNota $reg.Final) {

            foreach ($item in $salida) {
                if ($item.Materia -eq $id) {
                    $item.Recursan++
                    $esNuevo = 0
                    break
                }
            }

            if ($esNuevo) {
                $materia.Recursan++
                $salida += $materia
            }

            continue
        }

        if (recupera $reg.PrimerParcial $reg.SegundoParcial $reg.RecuParcial $reg.RecuNota) {

            foreach ($item in $salida) {
                if ($item.Materia -eq $id) {
                    $item.Recuperan++
                    $esNuevo = 0
                    break
                }
            }

            if ($esNuevo) {
                $materia.Recuperan++
                $salida += $materia
            }

            continue
        }

        if (abandona $reg.PrimerParcial $reg.SegundoParcial $reg.RecuNota) {

            foreach ($item in $salida) {
                if ($item.Materia -eq $id) {
                    $item.Abandonaron++
                    $esNuevo = 0
                    break
                }
            }

            if ($esNuevo) {
                $materia.Abandonaron++
                $salida += $materia
            }

            continue
        }
    }
}

$salida | Export-Csv "$PSScriptRoot/salida1.csv" -NoTypeInformation


# ------------------------FIN MAIN-------------------------------------------------------------------------#
#
# -----------------------FIN SCRIPT------------------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------#