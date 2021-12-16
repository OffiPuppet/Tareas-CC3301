=============================================================
Esta es la documentación para compilar y ejecutar su solucion
=============================================================

Se está ejecutando el comando: less README.txt

***************************
*** Para salir: tecla q ***
***************************

Para avanzar a una nueva página: tecla <page down>
Para retroceder a la página anterior: tecla <page up>
Para avanzar una sola línea: tecla <enter>
Para buscar un texto: tecla / seguido del texto (/...texto...)
         por ejemplo: /ddd

-----------------------------------------------

Programe su solucion en el archivo sumar-str.c.  Ya hay una plantilla en
sumar-str.c.plantilla.

Compile y ejecute bajo Debian 11 en un terminal con estos comandos:

make run
make run-san

El primer comando compila y ejecuta para x86 de 64 bits con opciones de
optimizacion, el segundo comando compila y ejecuta con chequeos para detectar
errores de manejo de memoria u operaciones indefinidas.

Si su solución funciona correctamente termina con el mensaje 'Felicitaciones'.

Si una de las compilaciones falla, lea atentamente el mensaje de error y
edite el archivo con el código fuente para corregir el problema.

----------------------------------------------

Depuración con ddd

Si la compilación es exitosa pero uno de los comandos de más arriba no
termina con felicitaciones, deberá depurar su tarea con uno de estos
comandos que lanza ddd para averiguar qué falló:

make ddd
make ddd-san

Ejecute con el botón run.

Si falla uno de los tests, la ejecución se detendrá justo después del
test que falló.  Vaya al menú Status de ddd y seleccione Backtrace para abrir
una ventana con las funciones en ejecución.  Seleccione la función main o
sumarStr para determinar con que parámetros no funcionaron.

Es normal que ddd abra una ventana de diálogo para reclamar porque no
encuentra los fuentes de algunas funciones de biblioteca, como exit.
Ignore el problema presionando el botón OK y continúe.

A sanitize no le gusta correr bajo ddd/gdb.  Si después de mostrar el mensaje

Felicitaciones: su solucion es correcta

aparece el error: LeakSanitizer has encountered a fatal error
es normal lamentablemente.  Esto trae como consecuencia que bajo ddd
sanitize no va a detectar memory leaks.  Para los memory-leaks tiene
que ejecutar make run-san.
