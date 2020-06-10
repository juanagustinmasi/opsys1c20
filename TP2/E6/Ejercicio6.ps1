# -----------------------ENCABEZADO------------------------------------------------------------------------#
#
# Nombre del script: Ejercicio6
# Trabajo practico: 1
# Ejercicio: 6
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


<#
.SYNOPSIS
       Este Script permite realizar la suma de todos los números fraccionarios contenidos en un archivo.

.DESCRIPTION
        Se ingresa el path del archivo a correrle el script como unico parametro, para que se realice la suma de las fracciones contenidas
        Guardando en un archivo salida.out (en el mismo directorio) el resultado e informandolo por salida standard

.INPUTS 
        -Path: Ruta del archivo de entrada (relativa o absoluta).
        
.EXAMPLE
  Ejemplo de ejecucion:> .\ejercicio6.ps1 -Path .\enunciado.in

                       >2741/360
.EXAMPLE
  Ejemplo de ejecucion:> .\ejercicio6.ps1 .\ceros.in

                       >0
#>



# --------------------------FUNCIONES-----------------------------------------------------------------------#



Param(

   [parameter(Mandatory = $true)] [string] $Path,
   [parameter(ValueFromRemainingArguments=$true)] $Otros=$NULL
	
)




function simplificar($numerador, $denominador, $dividir, $signo){
    $numerador /= $dividir
    $denominador /= $dividir
    
    if($signo -eq 1) {  
            $numerador *= -1
    }
    

    grabar $numerador $denominador  #Grabo archivo
}

function grabar($numerador, $denominador){

    if($denominador -eq 1 -or $numerador -eq 0) { 
        Write-Host -ForegroundColor DarkCyan "$numerador"
        Write-Output "$numerador" > salida.out
    }
    else {
        Write-Host -ForegroundColor DarkCyan "$numerador/$denominador"   
        Write-Output "$numerador/$denominador" > salida.out
    }
}

#-----------------------FIN FUNCIONES---------------------------------------------------------------------#
#
#-------------------AYUDA Y VALIDACIONES------------------------------------------------------------------#

if($Otros -ne $NULL) { #validacion de parametros
    write-host -ForegroundColor red "ERROR. Se pasaron mas parametros de los que eran necesarios"
    exit
}

$existe = Test-Path $Path 

if ($existe -ne $true) {
   write-host -ForegroundColor red "Archivo de entrada inexistente, por favor ingrese un archivo valido"
   exit 1
}

$valor = Get-Content $Path #Guardo el contenido del archivo

if($valor -eq $NULL) { 
    Write-Host -ForegroundColor red "0. Archivo vacio"
    Write-Output "0" > salida.out
    exit 1;
}

# -------------------FIN AYUDA Y VALIDACIONES---------------------------------------------------------------#
#
# ----------------------------MAIN--------------------------------------------------------------------------#

$vectorFracciones = @() #Creo vector donde voy a guardar cada fraccion
$vectorFracciones += ($valor).Split(',') #separador definido ","

$signo = 0
$contador = 0

foreach( $i in $vectorFracciones ){
    
    $fraccion = @()
    $fraccion += ($i).Split('/') #Separo en numerador y denominador

    if($fraccion[0] -match ':') { #Caso fraccion Mixta
        
        $mixta = @()
        $mixta += $fraccion[0].Split(':') #separo la fraccion convirtiendo de fraccion mixta
        if ([int]$mixta[0] -lt 0 )
        {
        $numerador = ([int]$mixta[0] * (-1) * [int]$fraccion[1]) + [int]$mixta[1]  #En fraccion impropia
        $denominador = [int]$fraccion[1] 
        $signo++
        }
        else{
        $numerador = ([int]$mixta[0] * [int]$fraccion[1]) + [int]$mixta[1]  #En fraccion impropia
        $denominador = [int]$fraccion[1] 
        }
    }
    else {

        if($fraccion[1] -eq $NULL) { #Caso entero
            $denominador = 1 #asigno 1 al denominador para poder operar en caso vacio
        }
        else {
            $denominador = [int]$fraccion[1]
        }
	    
        $numerador = [int]$fraccion[0]  
    }

    if($contador -gt 0) {
    
        $numRes = ($numerador * $denominador2) + ($denominador * $numerador2) #Result numerador 
        $denRes = $denominador * $denominador2 #Result denominador
        $numerador2 = $numRes
        $denominador2 = $denRes
    }
    else {
        $numerador2 = $numerador
        $denominador2 = $denominador
    }
    $contador++
}


$numerador = $numerador2
$denominador = $denominador2

#normalizo el signo 
if ($numerador -lt 0) { 
    $numerador *= -1;
    $signo++

}

if ($denominador -lt 0) { 

    $denominador *= -1
    $signo++

}

$numAux = $numerador
$denAux = $denominador


#calculo el mcd
while($denAux -ne 0){
    $r = $numAux % $denAux
      $numAux = $denAux
      $denAux = $r
  }
  $dividir = $numAux


#llamo a simplificar y grabo
simplificar $numerador $denominador $dividir $signo




# ------------------------FIN MAIN-------------------------------------------------------------------------#
#
# -----------------------FIN SCRIPT------------------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------# 