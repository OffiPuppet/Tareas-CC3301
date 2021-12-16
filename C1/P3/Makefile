# Para revisar las opciones de compilacion y ejecucion,
# ingrese en el terminal el comando: make
#
#
PROB=prev
TEST=test
HEADER=$(PROB)
BIN=bin-g
OPT=-g

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

MAK=make --no-print-directory

readme:
	@less README.txt

run: bin
	@echo "Midiendo cuanto demora la solucion del profesor"
	rm -f tiempo-ref.txt
	chmod +x prof.ref
	./prof.ref ref > /dev/null
	@echo "Tiempo de referencia = " `cat tiempo-ref.txt`
	@echo "Verificando y comparando con su solucion"
	./$(TEST).bin `cat tiempo-ref.txt`

run-g: bin-g
	./$(TEST).bin-g

run-san: bin-san
	./$(TEST).bin-san

run32: bin32
	./$(TEST).bin32

run32-g: bin32-g
	./$(TEST).bin32-g

runrv32: binrv32
	$(QEMU32) $(TEST).binrv32

runrv: binrv
	$(QEMU) $(TEST).binrv

ddd: bin-g
	ddd --command prolog.gdb $(TEST).bin-g &

ddd32: bin32-g
	ddd --command prolog.gdb $(TEST).bin32-g &

ddd-san: bin-san
	ddd --command prolog.gdb $(TEST).bin-san &

zip:
	@$(MAK) clean
	@echo ==== make run ==== | tee resultados.txt
	@$(MAK) run |& tee -a resultados.txt
	@echo ==== make run-g ==== | tee -a resultados.txt
	@$(MAK) run-g |& tee -a resultados.txt
	@echo ==== make run-san ==== |& tee -a resultados.txt
	@$(MAK) run-san |& tee -a resultados.txt
	@echo ==== make run32 ==== | tee -a resultados.txt
	@$(MAK) run32 |& tee -a resultados.txt
	@echo ==== zip ====
	zip -r $(PROB).zip resultados.txt $(PROB).c
	@echo "Entregue por u-cursos el archivo $(PROB).zip"

bin:
	@$(MAK) "OPT=-O -DBENCH=1" BIN=bin $(TEST).$@

bin-g:
	@$(MAK) OPT=-g BIN=bin-g $(TEST).$@

bin-san:
	@$(MAK) "OPT=-g -fsanitize=address -fsanitize=undefined -DSANITIZE" BIN=bin-san $(TEST).$@

bin32:
	@$(MAK) "OPT=-O -m32" BIN=bin32 $(TEST).$@
	
bin32-g:
	@$(MAK) "OPT=-g -m32" BIN=bin32-g $(TEST).$@
	
binrv:
	@$(MAK) "CC=$(RVCC)" "OPT=-O -DRISCV=1" BIN=binrv $(TEST).$@ 

binrv32:
	@$(MAK) "CC=$(RV32CC)" "OPT=-O -DRISCV=1" BIN=binrv32 $(TEST).$@ 

$(TEST).$(BIN): $(PROB).c $(TEST).c $(HEADER).h
	$(CC) $(OPT) $(CFLAGS) $(PROB).c $(TEST).c $(LDLIBS) -o $@

clean:
	rm -f *.o *.bin* resultados.txt tiempo-ref.txt
