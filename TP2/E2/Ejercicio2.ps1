<#
.Synopsis
   Muestra por pantalla una serie de datos obtenidos a partir de procesar los archivos de log del directorio que recibe por parámetro.
.DESCRIPTION
    Cada archivo de log se corresponde con los registros de una semana de llamadas de un call center.
    Se mostrará por pantalla los siguientes datos:
      -Promedio de tiempo de las llamadas realizadas por día.
      -Promedio de tiempo y cantidad por usuario por día.
      -Los 3 usuarios con más llamadas en la semana.
      -Cuántas llamadas no superan la media de tiempo por día y el usuario que tiene más llamadas por debajo de la media en la semana.
    No se realizará búsqueda recursiva sobre el directorio, y el mismo debe tener permisos de lectura.   
.EXAMPLE
   C:\PS> 
    .\Ejercicio2.ps1 -Path ./CasosDePruebaEJ2
.NOTES
    Authors:
	    Daiana Gomez Nespola, DNI 38005120
	    Juan Masi, DNI 37981647
        Guido Delbo, DNI 36982261
	    Nicolas Fortunato, DNI 37864810
	    Damian Perez, DNI 35375255
    
    Ejercicio2.ps1 - TP N°2 - PowerShell -  Ejercicio 2
    Entrega N° 1 

#>
Param (
 [Parameter(Position = 1, Mandatory = $true)]
 [ValidateNotNullOrEmpty()]
 [ValidateLength(2,256)]
 [ValidateScript({Test-Path $_ -PathType "Container"})]
 [String] $Path
)

gci $Path -File | Where-Object {$_.length -gt 0} | ForEach-Object { 

    Write-Output ""
    Write-Output "*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*"
    Write-Output "Resultados de la semana correspondiente al archivo: $_"
    Write-Output "*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*"
    Write-Output ""

    $listaDeLlamadas=New-Object System.Collections.ArrayList
    $i = 0

    $textoArchivo =$(Get-Content $_.FullName | %{
        New-Object psobject -Property @{
            Fecha = ($_).Split("_")[0].Trim(" ")
            Usuario = ($_).Split("_")[1].Trim(" ")
        }
    })
    
    $textoArchivo | Sort-Object -Property Usuario, Fecha | ForEach-Object {
        $i++
        if( ($i % 2) -ne 0 ) {
            #comienza la llamada
            $usuario=$_.Usuario
            $inicioLlamada=Get-Date -Date $_.Fecha
        }else{
            #termina la llamada
            $finllamada=Get-Date -Date $_.Fecha
            $dia=Get-Date -Date $inicioLlamada -Format "dd-MM-yyyy"
            
            $obj = New-Object -TypeName psobject
            $obj | Add-Member -MemberType NoteProperty -Name Usuario -Value $usuario
            $obj | Add-Member -MemberType NoteProperty -Name Fecha -Value $dia
            $obj | Add-Member -MemberType NoteProperty -Name Duracion -Value $finllamada.Subtract($inicioLlamada).TotalSeconds
            $listaDeLlamadas.Add($obj) > $null
        }
        
    } 
    
    #Usuarios con mas llamadas en la semana
    $top3 = $($listaDeLlamadas | Group-Object -Property Usuario | Sort-Object -Property Count -Descending | Select-Object -First 3 -Property @{N="Usuario";E={$_.Name}}, @{N="Cantidad";E={$_.Count}})
    
    #Promedio de las llamadas por Dia
    $promXDia =$($listaDeLlamadas | Group-Object -Property Fecha | %{
        New-Object psobject -Property @{
            Fecha = $_.Name
            PromedioDuracion = [math]::Truncate(($_.Group | Measure-Object -Property Duracion -Average).Average)
        }
    })

    #promedio de tiempo y cantidad de llamadas de usuario por dia
    $promXUsuario =$($listaDeLlamadas | Group-Object -Property Usuario | %{
        New-Object psobject -Property @{
            Usuario = $_.Name
            Cantidad = $_.Count
            PromedioDuracion  = [math]::Truncate(($_.Group | Measure-Object -Property Duracion -Average).Average)
        }
    })

    $mediaSemanal = [math]::Truncate($($listaDeLlamadas | Measure-Object -Property Duracion -Average | Select-Object -Property Average).Average)

    $llamadasDebajoDeLaMedia=New-Object System.Collections.ArrayList

    $promXDia| Sort-Object -Property Fecha | ForEach-Object {
        $obj1 = $_
        $llamadasDebajoDeLaMedia.Add($($listaDeLlamadas | Where-Object { $_.Fecha -eq $obj1.Fecha -and $_.Duracion -lt $obj1.PromedioDuracion} | Group-Object -Property Fecha | Select-Object @{Name="Dia";Expression={$_.Name}}, @{Name="Cantidad";Expression={$_.Count}})) > $null
    }

    $usuarioConMasBajoLaMedia = $($listaDeLlamadas | Where-Object { $_.Duracion -lt $mediaSemanal} | Group-Object -Property Usuario | %{
        New-Object psobject -Property @{
            Usuario = $_.Name
            Cantidad = $_.Count
        }
    } | Sort-Object -Property Cantidad -Descending | Select-Object -First 1 )

    Write-Output "Promedio de llamadas por Dia:"
    Write-Output "-----------------------------"
    $promXDia | Format-List Fecha, @{N="Promedio";E={[String]$_.PromedioDuracion + ' segundos(' + [String] $([Math]::Truncate($_.PromedioDuracion/60)) + ' minutos '+ [String]$($_.PromedioDuracion%60) + 'segundos)'}}

    Write-Output "Promedio de tiempo y cantidad de llamadas por usuario por dia:"
    Write-Output "--------------------------------------------------------------"
    $promXUsuario | Format-List Usuario, Cantidad, @{N="Promedio";E={[String]$_.PromedioDuracion + ' segundos(' + [String] $([Math]::Truncate($_.PromedioDuracion/60)) + ' minutos '+ [String]$($_.PromedioDuracion%60) + 'segundos)'}}

    Write-Output "Usuarios con mas llamadas en la semana:"
    Write-Output "---------------------------------------"
    $top3 | Format-List 

    Write-Output "Cantidad de llamadas debajo de la media por dia:"
    Write-Output "------------------------------------------------"
    $llamadasDebajoDeLaMedia | Format-List 

    Write-Output "Usuario con mas llamadas bajo la media de la semana de $mediaSemanal segundos($([Math]::Truncate($mediaSemanal/60)) minutos $($mediaSemanal%60) segundos):"
    Write-Output "-------------------------------------------------------------------------------------------"
    $usuarioConMasBajoLaMedia | Format-List 

}