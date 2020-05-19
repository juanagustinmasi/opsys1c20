#! /bin/bash
#
# -----------------------ENCABEZADO------------------------------------------------------------------------#
#
# Nombre del script: Ejercicio2
# Trabajo practico: 1
# Ejercicio: 2
# Entrega: 3ra
# Integrantes:
#	    Daiana Gomez Nespola, DNI 38005120
#	    Juan Masi, DNI 37981647
#     Guido Delbo, DNI 36982261
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


function Help()
{
 echo "Ayuda del script Ejercicio2:"
 echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~"
 echo "Este script muestra por pantalla una serie de datos obtenidos a partir de procesar los archivos de los del directorio que recibe por parámetro."
 echo "Cada archivo de log se corresponde con los registros de una semana de llamadas de un call center."
 echo "Se mostrará por pantalla los siguientes datos:"
 echo "  -Promedio de tiempo de las llamadas realizadas por día."
 echo "  -Promedio de tiempo y cantidad por usuario por día."
 echo "  -Los 3 usuarios con más llamadas en la semana."
 echo "  -Cuántas llamadas no superan la media de tiempo por día y el usuario que tiene más llamadas por debajo de la media en la semana."
 echo "La forma de llamar al script es : Ejercicio2.sh -f path_directorio"
 echo "No se realizará búsqueda recursiva sobre el directorio, y el mismo debe tener permisos de lectura."
 exit 0 
}

# ------------------------FIN FUNCIONES---------------------------------------------------------------------#
#
# -------------------AYUDA Y VALIDACIONES------------------------------------------------------------------#

if [ "$1" == "-?" ] || [ "$1" == "-h" ] || [ "$1" == "-help" ]; then
 Help
fi

if [ $# -lt 2 ] || [ $# -gt 3 ]; then
 echo "La cantidad de parámetros es incorrecta."
 opcionesAyuda
 exit 1
fi

if [ ! $1 == "-f" ]; then
 echo "Los parámetros ingresados no son correctos."
 opcionesAyuda
 exit 2
fi

if ! test -d $2; then
 echo "El parámetro ingresado no es un directorio,"
 opcionesAyuda
 exit 3
fi

if ! test -r $2; then
 echo "Error. El directorio $2 debe tener permisos de lectura."
 exit 4
fi

# -------------------FIN AYUDA Y VALIDACIONES---------------------------------------------------------------#
#
# ----------------------------MAIN--------------------------------------------------------------------------#


for archivo in $(find $2 -maxdepth 1 -type f -readable); do
 primeraLinea=0
 oldIFS=IFS
 IFS=" "
 declare -a usuariosArray
 declare -a datesArray
 declare -a timeArray
 declare -A cantLlamadasxUsuario
 declare -A cantLlamadasxDia
 declare -A sumaDuracionxDia  
 declare -A promDeDuracionXdia
 declare -a duracionXllamada
 declare -a fechaXllamada
 declare -a usuarioxLlamada
 declare -A cantBajoPromXdia
 declare -A cantBajoPromSemXusuario
 i=0
 duracionSemComp=0
 #valido que el archivo no este vacio
 if [[ -s $archivo ]];then
 (while read fecha hora sep usuario;
 do
   if [ $primeraLinea -eq 0 ];then
    primeraLinea=1

    echo "*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*"
    echo "Resultados de la semana correspondiente al archivo: "$archivo
    echo "*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*"

   fi
    datesArray[$i]=$fecha
    timeArray[$i]=$hora
    usuariosArray[$i]=$usuario
   i=$((i+1))
 done<<< $(cat $archivo | sort -k3 -k1n -k2n)
 
 if [ $primeraLinea -gt 0 ];then
 
  for((x=0, y=0;x<i;x=x+2, y++)); do

   #Guardamos el usuario que realizó la llamada
   usuarioxLlamada[$y]=${usuariosArray[$x]}
   
   fechaIni=${datesArray[$x]}
   horaIni=${timeArray[$x]}
   #El día de la llamada es el día que se inicia, no importa si finaliza al dia siguiente
   fechaXllamada[$y]=$fechaIni
   
   datetimeInicio=`date -d "$fechaIni $horaIni" "+%s"`
   fechaFin=${datesArray[$((x+1))]}
   horaFin=${timeArray[$((x+1))]}
   datetimeFin=`date -d "$fechaFin $horaFin" "+%s"`
   

   duracionXllamada[$y]=$(($datetimeFin - $datetimeInicio)) 
   cantLlamadasxUsuario[${usuariosArray[$x]}]=$((${cantLlamadasxUsuario[${usuariosArray[$x]}]}+1))
   cantLlamadasxDia[$fechaIni]=$((${cantLlamadasxDia[$fechaIni]}+1))
   sumaDuracionxDia[$fechaIni]=$((${sumaDuracionxDia[$fechaIni]}+${duracionXllamada[$y]}))
   duracionSemComp=$((duracionSemComp+${duracionXllamada[$y]}))
  done
  promDeDuracionXsem=$((duracionSemComp/${#duracionXllamada[@]}))

  echo "Promedio de tiempo de las llamadas por día:"
  
  for key in "${!cantLlamadasxDia[@]}"; do
   sumaDuracionXdia=${sumaDuracionxDia[$key]}
   cantxDia=${cantLlamadasxDia[$key]}
   promDeDuracionXdia[$key]=$((sumaDuracionXdia/cantxDia)) 
   cantBajoPromXdia[$key]=0 #pongo en 0 el contador de las llamadas bajo el promedio del dia
   echo "Dia: $key Promedio de duracion de las llamadas:${promDeDuracionXdia[$key]} segundos($((${promDeDuracionXdia[$key]}/60)) minutos $((${promDeDuracionXdia[$key]}%60)) segundos)."   
    
  done
  
  echo "---------------------------------------"
  echo "Promedio de tiempo y cantidad de llamadas por usuario por día:"
  cant=0
  duracion=0
  for((x=0;x<${#usuarioxLlamada[@]};x++)); do
  #Los datos los tengo ordenados por usuario/fecha-hora
    usuario=${usuarioxLlamada[$x]}
    usuarioProx=${usuarioxLlamada[$((x+1))]}
    cant=$((cant+1))
    duracion=$((duracion+${duracionXllamada[$x]}))
      
    if [ "${usuarioxLlamada[$x]}" != "${usuarioxLlamada[$((x+1))]}" ];then
     echo "Usuario: $usuario"
     prom=$((duracion/cant))
     echo "  Cantidad: $cant Promedio: $prom segundos($((prom/60)) minutos $((prom%60)) segundos)"          
     cant=0
     duracion=0
     prom=0
    fi
  done 
  
  primero=""
  cantPrimero=0
  segundo=""
  cantSegundo=0
  tercero=""
  cantTercero=0
  for key in "${!cantLlamadasxUsuario[@]}"; do
    if [ ${cantLlamadasxUsuario[$key]} -ge $cantPrimero ];then
     tercero=$segundo
     cantTercero=$cantSegundo
     segundo=$primero
     cantSegundo=$cantPrimero
     primero=$key
     cantPrimero=${cantLlamadasxUsuario[$key]} 
    else
     if [ ${cantLlamadasxUsuario[$key]} -ge $cantSegundo ];then
      tercero=$segundo
      cantTercero=$cantSegundo
      segundo=$key
      cantSegundo=${cantLlamadasxUsuario[$key]}    
     else
      if [ ${cantLlamadasxUsuario[$key]} -ge $cantTercero ];then   tercero=$key
       cantTercero=${cantLlamadasxUsuario[$key]}    
      fi
     fi
    fi
    #Ponemos en 0 el contador de llamadas bajo el promedio por usuario
    cantBajoPromSemXusuario[$key]=0
  done
  
  echo "---------------------------------------"
  echo "Usuarios con más llamadas en la semana:"
  echo "1- $primero"
  if [ $cantSegundo -gt 0 ];then 
   echo "2- $segundo" 
  fi
  if [ $cantTercero -gt 0 ];then 
   echo "3- $tercero" 
  fi
  
  for((x=0;x<${#fechaXllamada[@]};x++)); do
   fecha=${fechaXllamada[$x]}
   if [ ${duracionXllamada[$x]} -le ${promDeDuracionXdia[$fecha]} ];then
    cantBajoPromXdia[$fecha]=$((${cantBajoPromXdia[$fecha]}+1))
   fi
   if [ ${duracionXllamada[$x]} -le $promDeDuracionXsem ];then 
    usuario=${usuarioxLlamada[$x]}
    cantBajoPromSemXusuario[$usuario]=$((${cantBajoPromSemXusuario[$usuario]}+1))                    
   fi
  done
  
  echo "---------------------------------------"
  echo "Cantidad de llamadas por día que no superan la media:"
  for key in "${!cantBajoPromXdia[@]}"; do
   echo "Dia: $key Cant. llamadas: ${cantBajoPromXdia[$key]}"   
  done
  
  usuarioMasLlamadasBajoMediaSemanal=""
  cantMax=0
  for key in "${!cantBajoPromSemXusuario[@]}"; do
   if [ $cantMax -le ${cantBajoPromSemXusuario[$key]} ];then
    cantMax=${cantBajoPromSemXusuario[$key]}
    usuarioMasLlamadasBajoMediaSemanal=$key
   fi    
  done
  echo "---------------------------------------"
  echo "Usuario con mayor cantidad de llamadas en la semana que no superan la media semanal de $promDeDuracionXsem segundos($((promDeDuracionXsem/60)) minutos $((promDeDuracionXsem%60)) segundos):"
  echo "$usuarioMasLlamadasBajoMediaSemanal"
  
  
 fi  
 )   
 fi  
done

IFS=oldIFS
# ------------------------FIN MAIN-------------------------------------------------------------------------#
#
# -----------------------FIN SCRIPT------------------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------#