Param (
 <#[Parameter(Position = 1, Mandatory = $false)]
 [String] $pathsalida = ".\procesos.txt ",
 [int] $cantidad = 3
#>
 [Parameter(Position = 1, Mandatory = $true)]
 [ValidateLength(2,256)]
 [ValidateScript({Test-Path $_ -PathType Leaf})]
 [String] $pathsalida,
 [Parameter(Position = 2)]
 #[ValidateRangeKind("Positive")]
 [int] $cantidad = 3
)
$existe = Test-Path $pathsalida
if ($existe -eq $true) {
 $listaproceso = Get-Process
 foreach ($proceso in $listaproceso) {
 $proceso | Format-List -Property Id,Name >> $pathsalida
 }
 for ($i = 0; $i -lt $cantidad ; $i++) {
 Write-Host $listaproceso[$i].Name - $listaproceso[$i].Id
 }
} else {
 Write-Host "El path no existe"
}
<#
Responder:
1. ¿Cual es el objetivo del script?
 Su objetivo es escribir en el archivo que recibe por parametro los procesos que estan corriendo en ese momento.
 y muestra por pantalla los primeros tres procesos de esa lista, o tantos como reciba por parametro.
2. ¿Agregaria alguna otra validacion a los parametros?
 Si, pondria $pathsalida como mandatorio y validaria su longitud, y que $cantidad pueda accederse por la position.
 Se agregan las validaciones en comentarios en el codigo
3. ¿Que sucede si se ejecuta el script sin ningun parametro?
 Por pantalla se muestra "El path no existe" ya que valida directamente si $pathsalida existe, sin consultar que sea nulo.
 Con los cambios que realice ya no muestra ese mensaje sino que devuelve un error de validacion del parametro pathsalida.
#>