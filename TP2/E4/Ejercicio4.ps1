# -----------------------ENCABEZADO------------------------------------------------------------------------#
#
# Nombre del script: Ejercicio4
# Trabajo practico: 2
# Ejercicio: 4
# Entrega: 1era
# Integrantes:
#	    Daiana Gomez Nespola, DNI 38005120
#	    Juan Masi, DNI 37981647
#       Guido Delbo, DNI 36982261
#	    Nicolas Fortunato, DNI 37864810
#	    Damian Perez, DNI 35375255

<#
.SYNOPSIS
Este Script evalua todos los logs, dejando solamente los de la última semana de cada empresa.
Los archivos anteriores los comprime en un directorio y los elimina.
De acuerdo a los parametros que se pasen se puede revisar los logs de todas las empresas o de una especifica
Los archivos de logs deben tener el siguiente un formato especifico.El formato debe ser Empresa-numeroDeLaSemana.log

.DESCRIPTION
Parametro Directorio: Directorio en el que se encuentran los archivos de log
Parametro DirectorioZip: Directorio en el que se generarán los archivos comprimidos de los clientes
Parametro Empresa: Nombre de la empresa a procesar. Es un parametro opcional

.EXAMPLE
Ejemplo:> ./Ejercicio4.sh -Directorio "CasosDePrueba/ArchivosLog/Enunciado" -DirectorioZip "CasosDePrueba/ArchivosComprimidos"
Ejemplo:> ./Ejercicio4.sh -Directorio "CasosDePrueba/ArchivosLog/Enunciado" -DirectorioZip "CasosDePrueba/ArchivosComprimidos" -Empresa "unlam"

#>

# ----------------------FIN ENCABEZADO---------------------------------------------------------------------#
#
# --------------------------SCRIPT-------------------------------------------------------------------------#

# -------------------AYUDA Y VALIDACIONES------------------------------------------------------------------#

param (
	[Parameter(Mandatory=$true)][ValidateNotNullOrEmpty()][string] $Directorio,
 	[Parameter(Mandatory=$true)][ValidateNotNullOrEmpty()][string] $DirectorioZip,
	[Parameter(Mandatory=$false)][string] $Empresa
)

if(!( Test-Path -path $Directorio )) {
	Write-Host "El path donde se encuentran los archivos de log no es un directorio valido."
	exit 0;
}

if(!( Test-Path -path $DirectorioZip )) {
	Write-Host "El path donde se van a comprimir los archivos de log no es un directorio valido."
	exit 1;
}

# -------------------FIN AYUDA Y VALIDACIONES---------------------------------------------------------------#
#
# ----------------------------MAIN--------------------------------------------------------------------------#

$cantPar = ($psboundparameters.Count + $args.Count)
	
if( $cantPar -ne 3 )
{
	$EMPRESA_INICIAL=""
	$EXTENSION=".zip"   

	$files = Get-ChildItem  -Path $Directorio -Name -File | Select-string "[a-zA-Z]+-[0-9]+.log" | Sort-Object -CaseSensitive -Descending {($_ -split "-|\.")[0]},{[int]($_ -split "-|\.")[1]}
	foreach($file in $files)
	{	
		$EMPRESA = ($file -split "-|\.")[0]
				
		if( $EMPRESA -eq $EMPRESA_INICIAL )
		{
			Write-Host "$file"
			Compress-Archive -Path "$Directorio/$file" -Update -DestinationPath "$directorioZip/$EMPRESA$EXTENSION"
			Remove-Item -Path "$Directorio/$file"
		}else{
			$EMPRESA_INICIAL = ($file -split "-|\.")[0]
		}
	}
}else{
	$VERSION=1
	$destino="$Empresa"
	$destino+=".zip" 

    	$files = Get-ChildItem  -Path $Directorio -Name -File | Select-string "[a-zA-Z]+-[0-9]+.log" | Sort-Object -CaseSensitive -Descending {($_ -split "-|\.")[0]},{[int]($_ -split "-|\.")[1]}
	foreach($file in $files)
	{
		$EMPRESA_INICIAL=($file -split "-|\.")[0]
		
		if( $EMPRESA_INICIAL -eq $Empresa )
		{
			if ( $VERSION -ne 0 )
			{
				$VERSION=0
			}else{
				Write-Host "$file"
				Compress-Archive -Path "$Directorio/$file" -Update -DestinationPath "$directorioZip/$destino"
				Remove-Item -Path "$Directorio/$file"
			}
		}		
	}
}	
	
# ------------------------FIN MAIN-------------------------------------------------------------------------#
#
# -----------------------FIN SCRIPT------------------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------#
