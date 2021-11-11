# Programe su tarea modificando solo el archivo recortar.c
# Para revisar las opciones de compilacion y ejecucion,
# ingrese en el terminal el comando: make
#
SHELL=bash -o pipefail

INCLUDE=
CFLAGS=-Wall -Werror -pedantic -std=c18 $(INCLUDE)
LDLIBS=-lm

# Para Risc-V
RISCV = /opt/riscv
TARGET = riscv64-unknown-elf
RVCC= ${RISCV}/bin/${TARGET}-gcc -march=rv64gc
RV32CC= ${RISCV}/bin/${TARGET}-gcc -march=rv32imafc -mabi=ilp32f
QEMU=qemu-riscv64
QEMU32=qemu-riscv32

MAK=make -B --no-print-directory

readme:
	@less README.txt

run: test-recortar.run
run-g: test-recortar.run-g
ddd: test-recortar.ddd
bin-g: test-recortar.bin-g
run32: test-recortar.run32
run32-g: test-recortar.run32-g
ddd32: test-recortar.ddd32
bin32-g: test-recortar.bin32-g
runrv: test-recortar.runrv
runrv32: test-recortar.runrv32

.PRECIOUS: test-%.bin test-%.bin-g test-%.bin32 test-%.bin32-g test-%.binrv test-%.binrv32

test-%.bin: %.c test-%.c %.h
	$(CC) -O $(CFLAGS) $(LDFLAGS) $(*F).c test-$(*F).c $(LDLIBS) -o $@

test-%.bin-g: %.c test-%.c %.h
	$(CC) -g $(CFLAGS) $(LDFLAGS) $(*F).c test-$(*F).c $(LDLIBS) -o $@ 

test-%.bin32: %.c test-%.c %.h
	$(CC) -O -m32 $(CFLAGS) $(LDFLAGS) $(*F).c test-$(*F).c $(LDLIBS) -o $@

test-%.bin32-g: %.c test-%.c %.h
	$(CC) -g -m32 $(CFLAGS) $(LDFLAGS) $(*F).c test-$(*F).c $(LDLIBS) -o $@

test-%.binrv: %.c test-%.c %.h
	$(RVCC) -DRISCV=1 $(CFLAGS) $(LDFLAGS) $(*F).c test-$(*F).c $(LDLIBS) -o $@

test-%.binrv32: %.c test-%.c %.h
	$(RV32CC) -DRISCV=1 $(CFLAGS) $(LDFLAGS) $(*F).c test-$(*F).c $(LDLIBS) -o $@

%.ddd: %.bin-g
	ddd $^ &

%.ddd32: %.bin32-g
	ddd $^ &

%.run: %.bin
	@echo "Midiendo cuanto demora la solucion del profesor"
	rm -f tiempo-ref.txt
	./prof.binref ref | tee tiempo-ref.txt
	@echo "Verificando y comparando con su solucion"
	./$(*F).bin `cat tiempo-ref.txt`

%.run-g: %.bin-g
	./$(*F).bin-g

%.run32: %.bin32
	./$(*F).bin32

%.run32-g: %.bin32-g
	./$(*F).bin32-g

%.runrv: %.binrv
	$(QEMU) $(*F).binrv

%.runrv32: %.binrv32
	$(QEMU32) $(*F).binrv32

zip:
	@$(MAK) clean
	@echo ==== make run ==== | tee resultados.txt
	@$(MAK) run |& tee -a resultados.txt
	@echo ==== make run-g ==== | tee -a resultados.txt
	@$(MAK) run-g |& tee -a resultados.txt
	@echo ==== make run32 ==== | tee -a resultados.txt
	@$(MAK) run32 |& tee -a resultados.txt
	@echo ==== zip ====
	zip -r recortar.zip resultados.txt recortar.c
	@echo "Entregue por u-cursos el archivo recortar.zip"

clean:
	rm -f *.o  *.log *.bin *.bin-g *.bin32 *bin32-g *.binrv *.binrv32 resultados.txt tiempo-ref.txt recortar.zip
