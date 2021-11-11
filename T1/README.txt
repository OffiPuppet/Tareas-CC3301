==========================================================
Esta es la documentacion para compilar y ejecutar su tarea
==========================================================

Se esta ejecutando el comando: less README.txt

***************************
*** Para salir: tecla q ***
***************************

Para avanzar a una nueva pagina: tecla <page down>
Para retroceder a la pagina anterior: tecla <page up>
Para avanzar una sola linea: tecla <enter>
Para buscar un texto: tecla / seguido del texto (/...texto...)
         por ejemplo: /ddd

-----------------------------------------------

Requisitos que debe cumplir para poder entregar su tarea

Ejecute bajo Debian 11 en un terminal estos comandos:

make run
make run-g
make run32

Estos comandos compilan y ejecutan para x86 de 64 bits, x86 de 64 bits
con opciones de depuracion y x86 de 32 bits respectivamente.

Se exige que estos 3 comandos terminen con el mensaje 'Felicitaciones'.

Si una de las compilaciones falla, lea atentamente el mensaje de error,
edite el archivo recortar.c y resuelva el problema.

Si la compilacion es exitosa pero uno de los comandos de mas arriba no
termina con felicitaciones, debera depurar su tarea con uno de estos
comandos que lanza ddd para averiguar que fallo:

make ddd
make ddd32

Video con ejemplos de uso de ddd: https://youtu.be/FtHZy7UkTT4
Archivos con los ejemplos: https://www.u-cursos.cl/ingenieria/2020/2/CC3301/1/novedades/r/demo-ddd.zip

Como primer paso, en la ventana de comandos de ddd, ponga un breakpoint
en la funcion exit con este comando:

b exit

Luego ejecute con el boton run.

Si falla uno de los tests, la ejecucion se detendra justo despues del
test que fallo.  Vaya al menu Status de ddd y seleccione Backtrace para abrir
una ventana con las funciones en ejecucion.  Seleccione la funcion main
o recortar para determinar con que parametros recortar no
retorna el valor esperado.

Es normal que ddd abra una ventana de dialogo para reclamar porque no
encuentra los fuentes de exit.  Ignore el problema presionando el
boton OK y continue.

Si hace modificaciones a recortar.c, no necesita relanzar ddd, basta
con recompilar con 'make bin-g' o 'make bin32-g'.  Luego en la ventana
de ddd, rejecute con el boton run.  Ddd recargara automaticamente el
nuevo archivo binario ejecutable.  A veces ddd no recarga automaticamente
el archivo fuente.  Asegurese que lo haga yendo al menu Source y luego
seleccione Reload Source.

-----------------------------------------------

Resumen de opciones de compilacion/ejecucion:

*** Pruebas para la arquitectura x86 de 64 bits ***
make run : Compila y ejecuta con opciones de optimizacion
make run-g : Compila y ejecuta con opciones de depuracion
make ddd : Compila con opciones de depuracion y lanza ddd para depurar
make bin-g : Solo recompila con opciones de depuracion

*** Pruebas para la arquitectura x86 de 32 bits ***
make run32 : Compila y ejecuta con opciones de optimizacion para 
make run32-g : Compila y ejecuta con opciones de depuracion
make ddd32 : Compila con opciones de depuracion y lanza ddd para depurar
make bin32-g : Solo recompila con opciones de depuracion

*** Pruebas para la arquitectura RiscV ***
Solo para los que instalaron debian11mate.ova con VirtualBox.
En estos tests la compilacion genera archivos binarios ejecutables
en la arquitectura RiscV.  En la ejecucion se usa qemu para emular
el set de instrucciones de RiscV.

make runrv : Compila y ejecuta optimizado para RiscV de 64 bits
make runrv32: Compila y ejecuta optimizado para RiscV de 32 bits

-----------------------------------------------

Entrega de la tarea y los resultados

Ejecute en un terminal: make zip

Se ejecutaran automaticamente: make run, make run-g y make run32.
Si todos terminan con felicitaciones, se creara un archivo
recortar.zip que contiene el archivo recortar.c y otro archivo
resultados.txt con la salida de los 3 comandos de mas arriba.
Vaya a U-cursos -> CC3301 -> Tareas -> Tarea 1 y entregue el archivo
recortar.zip.

A continuacion es muy importante que descargue de U-cursos el mismo
archivo que subio y lo descomprima.  Luego examine los archivos recortar.c
y resultados.txt revisando que son los correctos.  Es frecuente que no lo sean
producto de un defecto de U-cursos.

Observacion: los ayudantes ejecutaran este mismo comando para probar su tarea
bajo Debian 11.  Si no cumple los requisitos su tarea sera rechazada.

-----------------------------------------------

Limpieza de archivos

make clean: hace limpieza borrando todos los archivos que se pueden volver
            a reconstruir a partir de los fuentes: *.o *.bin etc.

-----------------------------------------------

Acerca del comando make

El comando make sirve para automatizar el proceso de compilacion asegurando
recompilar el archivo binario ejecutable cuando cambio uno de los archivos
fuentes de los cuales depende.

La especificacion de los archivos de los cuales depende esta en el archivo
Makefile, que es muy complicado de entender.  Es el profesor el que los
elabora.

Durante la invocacion, make muestra los comandos que esta ejecutando,
junto a otros mensajes.  Ud.  puede invocar manualmente estos comandos
en el terminal y obtendra el mismo resultado, si los invoca en el
mismo orden.

A veces es util usar make con la opcion -n para que solo muestre
exactamente que comandos va a ejecutar, sin ejecutarlos de verdad.
Por ejemplo para determinar que comandos ejecutara make bin, esto
es lo que mostrara la invocacion de: make clean; make -n run

  cc -O  recortar.c test-recortar.c -lm -o test-recortar.bin
  echo "Midiendo cuanto demora la solucion del profesor"
  rm -f tiempo-ref.txt
  ./prof.binref ref | tee tiempo-ref.txt
  echo "Verificando y comparando con su solucion"
  ./test-recortar.bin `cat tiempo-ref.txt`

Tambien es util usar make con la opcion -B para forzar la recompilacion
de los fuentes a pesar de que no han cambiado desde la ultima compilacion.
Por ejemplo:

make -B run

Recompilara para generar el archivo test-recortar.bin
Si a continuacion invoca:

make run

No recompilara nada porque los fuentes no han cambiado desde la ultima
compilacion.  Solo ejecutara el binario.  Este ahorro no es relevante en
esta tarea que consiste en solo 2 archivos fuentes.  Pero un programa
grande puede consistir en cientos o miles de archivos y recompilar
todo puede tomar mucho tiempo.  Si se se cambia un solo archivo, make
solo recompilara lo que dependa de ese archivo economizando un tiempo
considerable.

Si ahora invoca:

make -B run

Se recompilara nuevamente todo, producto de la opcion -B, a pesar de que
los fuentes no han cambiado.
