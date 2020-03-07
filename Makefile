CC = gcc
CFLAGS = -Wall
DEPS = riscv-parse.h rvp-data.h rvp-utility-functions.h rvp-utility-json.h
OBJ = rvp-utility-json.o rvp-utility-functions.o riscv-parse.o main.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

parser: $(OBJ)
	gcc $(CFLAGS) -o $@ $^