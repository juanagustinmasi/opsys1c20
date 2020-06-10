#! /bin/bash
#
# -----------------------ENCABEZADO------------------------------------------------------------------------#
#
# Nombre del script: Ejercicio1
# Trabajo practico: 1
# Ejercicio: 1
# Entrega: 5ta
# Integrantes:
#	    Daiana Gomez Nespola, DNI 38005120
#	    Juan Masi, DNI 37981647
#     Guido Delbo, DNI 36982261
#	    Nicolas Fortunato, DNI 37864810
#	    Damian Perez, DNI 35375255
#
# ----------------------FIN ENCABEZADO--------------------------

ErrorS()
{
 echo "Error. La sintaxis del script es la siguiente:"
 echo "Para obtener la cantidad de líneas del archivo : $0 nombre_archivo L" 
 echo "Para obtener la cantidad de caracteres del archivo : $0 nombre_archivo C" 
 echo "Para obtener la longitud de la linea mas larga del archivo : $0 nombre_archivo M"
 #Se agrega esta línea para que no imprima dos veces el mensaje y salga del script directamente
 exit  
}

ErrorP()
{
 echo "Error. nombre_archivo debe tener permisos de lectura." 
}

#Se valida que no se hayan ingresado menos de dos parámetros
if test $# -lt 2; then
 ErrorS
fi

#Se valida el permiso de lectura sobre el archivo de prueba
if ! test -r $1; then
 ErrorP
#Se valida que se haya pasado por parámetro una opción valida y sino se muestra por pantalla una ayuda sobre la sintaxis
elif test -f $1 && (test $2 = "L" || test $2 = "C" || test $2 = "M"); then
 #Para la opción L se muestra por pantalla la cantidad de líneas del archivo
 if test $2 = "L"; then
  res=`wc -l $1`
  echo "Cantidad de líneas del archivo : $res" 
#Para la opción C se muestra la cantidad de caracteres del archivo
 elif test $2 = "C"; then
  res=`wc -m $1`
  echo "Cantidad de caracteres del archivo : $res" 
#Para la opción M se muestra el largo de la línea más larga del archivo
 elif test $2 = "M"; then
  res=`wc -L $1`
  echo "Largo de la línea más extensa del archivo : $res"
 fi
else
 ErrorS
fi

# Preguntas
# a) ¿Cual es el objetivo de este script?
#    Es devolver información sobre el archivo que se pasa por parámetro dependiendo de la opción seleccionada. 
#    Muestra la cantidad de líneas, de caracteres y la longitud de la línea más larga del archivo. 
# b) ¿Que parametros recibe?
#    El script recibe dos parametros: el primero: el path de un archivo, y el
#    segundo, una letra que indica el tipo de test a realizar sobre el archivo.
# c) Comentar el codigo segun la funcionalidad (no describa los comandos, indique la logica)
# d) Completar los "echo" con el mensaje correspondiente.
# e) ¿Que informacion brinda la variable "$#"? ¿Que otras variables similares conocen? Expliquelas.
#    La variable $# indica la cantidad de parametros recibidos por el script. Otras variables similares son:
#	  $0 : Contiene el nombre del script tal como es invocado.
#	  $* : El conjunto de todos los parametros en un solo argumento.
#	  $@ : El conjunto de argumentos, un argumento por parametro..
#	  $? : El codigo de retorno del ultimo comando.
#	  $$ : El PID del shell que ejecuta el script.
#	  $! : El PID del ultimo proceso ejecutado en segundo plano.
# f) Explique las diferencias entre los distintos tipos de comillas que se pueden utilizar en Shell scripts.
#    Hay tres tipos de comilla que pueden utilizarse: La comilla simple ('), la comilla doble (") y la comilla invertida (`).
#    El contenido entre las comillas simples se interpreta de manera literal, mientras que al utilizar comillas dobles interpretan
#    las variables y caracteres especiales, reemplazandolos por su contenido.En cambio, las comillas invertidas se utilizan
#    con ordenes o funciones, y se sustituyen por la salida del programa que esto entre las comillas invertidas.
# ------------------------FIN -------------------------------------------------------------------------#
# SISTEMAS OPERATIVOS | MARTES Y JUEVES - TURNO NOCHE | ANIO 2020 | PRIMER CUATRIMESTRE
# ---------------------------------------------------------------------------------------------------------#