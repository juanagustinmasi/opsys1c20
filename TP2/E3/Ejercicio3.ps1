# -----------------------ENCABEZADO------------------------------------------------------------------------#
#
# Nombre del script: Ejercicio3
# Trabajo practico: 2
# Ejercicio: 3
# Entrega: 4ta entrega
# Integrantes:
#	    Daiana Gomez Nespola, DNI 38005120
#	    Juan Masi, DNI 37981647
#       Guido Delbo, DNI 36982261
#	    Nicolas Fortunato, DNI 37864810
#	    Damian Perez, DNI 35375255
#
# ----------------------FIN ENCABEZADO---------------------------------------------------------------------#



<#
.SYNOPSIS
    Al crearse un nuevo archivo, se eliminan los que no sean de la ultima semana.
.DESCRIPTION
    En el momento en que se crea un nuevo archivo se desencadena un evento el cual recorre el directorio 
    pasado como parametro, y busca los archivos logs que cumplen con el formato NombreEmpresa-Semana.log
    los archivos de una misma empresa que sean viejos seran eliminados, quedando solo el mas reciente.
.PARAMETER Directorio
    Path del directorio a evaluar.
.EXAMPLE
    Ejercicio3.ps1 -Directorio ./directorio
#>



# ----------------------Validacion Parametros------------------------------------------------------------------#
Param(
    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript({
        if(-Not (Test-Path $_)){
            throw "El directorio no existe"
        }
        if(-Not (Test-Path $_ -PathType Container)){
            throw "El path debe corresponder a un directorio"
        }
        return $true
    })]
    [string]$Directorio
)
# ----------------------Setear el  watcher------------------------------------------------------------------#
$watcher = New-Object System.IO.FileSystemWatcher
$watcher.Path = Resolve-Path $Directorio
$watcher.IncludeSubdirectories = $false
$watcher.EnableRaisingEvents = $true
# ----------------------Registra evento, y define accion----------------------------------------------------#
Register-ObjectEvent $watcher -EventName Created -SourceIdentifier File.Created -Action {

    $path = Split-Path -Path $Event.SourceEventArgs.FullPath
    $lista_archivos = Get-ChildItem $path -Name -File | Select-string "[a-zA-Z]+-[0-9]+.log" | Sort-Object -CaseSensitive -Descending {($_ -split "-|\.")[0]},{[int]($_ -split "-|\.")[1]}
    $empresaActual = ""

    foreach ($archivo in $lista_archivos) {

        if ($empresaActual -ceq ($archivo -split "-|\.")[0]) {
            Remove-Item -Path "$path/$archivo"
        } else {
            $empresaActual=($archivo -split "-|\.")[0]
        }
}
}
# ----------------------Para eliminar el ID creado-----------------------------------------------------------#
#Unregister-Event File.Created
