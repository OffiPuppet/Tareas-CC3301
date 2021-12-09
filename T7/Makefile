# Para revisar las opciones de compilacion y ejecucion,
# ingrese en el terminal el comando: make
#
#

SHELL=bash -o pipefail

INCLUDE=
CFLAGS=-Wall -Werror -pedantic -std=c18 $(INCLUDE)

# Para Risc-V
RISCV = /opt/riscv
TARGET = riscv64-unknown-elf
CCRV= ${RISCV}/bin/${TARGET}-gcc -march=rv64gc
CCRV32= ${RISCV}/bin/${TARGET}-gcc -march=rv32imafc -mabi=ilp32f
QEMU=qemu-riscv64
QEMU32=qemu-riscv32
BIG := $(shell mktemp)

MAK=make --no-print-directory

readme:
	@less README.txt

%.bin: %.c
	gcc -O $(CFLAGS) $^ -o $@

%.bin-g: %.c
	gcc -g $(CFLAGS) $^ -o $@

%.bin-san: %.c
	gcc -g -fsanitize=address -DSANITIZE $(CFLAGS) $^ -o $@

run-san: mas-grande.bin-san make-big-file.bin mas-grande.bin-g
	@rm -f sal.txt
	@echo "Verificando con directorio test-dir"
	@echo "./mas-grande.bin-san test-dir"
	@./mas-grande.bin-san test-dir > sal.txt || (echo "codigo de retorno debio ser 0"; cat test-dir.txt; false)
	@cat sal.txt
	@diff sal.txt sal.ref > /dev/null || (echo "La salida debio ser:"; cat sal.ref; cat test-dir.txt false)
	@echo "Test aprobado" ; echo "-------------------"
	@echo "Verificando con archivo inexistente"
	@rm -f sal.txt err.txt
	@echo "./mas-grande.bin-san inexistente"
	@if ./mas-grande.bin-san inexistente > sal.txt 2>err.txt; then cat sal.txt err.txt; echo "codigo de retorno no debio ser 0"; cat inexistente.txt; false; fi
	@cat sal.txt ; cat err.txt
	@grep inexistente err.txt > /dev/null || (echo "el mensaje de error debio incluir inexistente"; false)
	@echo "Test aprobado" ; echo "-------------------"
	@echo "Verificando con un archivo de mas de 4 GB"
	@rm -f sal.txt
	./make-big-file.bin $(BIG)
	@echo ./mas-grande.bin-san $(BIG)
	@./mas-grande.bin-san $(BIG) > sal.txt || (cat sal.txt ; echo "codigo de retorno debio ser 0" ; cat big.txt; echo run $(BIG); false)
	@cat sal.txt
	@grep 68719476749 sal.txt >/dev/null || (echo "el tamanno debio ser 68719476749" ; cat big.txt; echo run $(BIG); false)
	@rm -f $(BIG)
	@echo "Test aprobado" ; echo "-------------------"
	@echo "Verificando directorio sin permiso de lectura"
	@rm -f sal.txt err.txt
	chmod a-rwx non-readable-dir
	@echo ./mas-grande.bin-san non-readable-dir
	@./mas-grande.bin-san non-readable-dir > sal.txt 2> err.txt || (cat sal.txt; cat err.txt ; echo "codigo de retorno debio ser 0" ; cat lectura.txt; false)
	@cat sal.txt
	@grep non-readable-dir err.txt || (echo "el mensaje de error debio incluir non-readable-dir" ; cat lectura.txt; false )
	@echo "Test aprobado" ; echo "-------------------"
	@echo "Verificando directorios /opt/riscv/bin /opt/riscv/libexec"
	@rm -f sal.txt
	@echo "./mas-grande.bin-san /opt/riscv/bin /opt/riscv/libexec"
	@./mas-grande.bin-san /opt/riscv/bin /opt/riscv/libexec > sal.txt || (echo "codigo de retorno debio ser 0"; cat riscv.txt; false)
	@cat sal.txt
	@diff sal.txt riscv.ref > /dev/null || (echo "La salida debio ser:"; cat riscv.ref; cat riscv.txt; false)
	@echo "Test aprobado"
	@echo "Felicitaciones: aprobo todos los tests"

%.ddd:
	$(MAK) "CFLAGS=-g $(CFLAGS)" $(*F).bin-g
	ddd --command $(*F).gdb $(*F).bin-g &

%.ddd-san:
	$(MAK) "CFLAGS=-g -fsanitize=address -DSANITIZE $(CFLAGS)" $(*F).bin-san
	ddd --command $(*F).gdb $(*F).bin-san &

clean:
	rm -f *.o *.log *.bin*
