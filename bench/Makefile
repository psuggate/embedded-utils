.PHONY:	clean bench run all

SRC	:= $(filter-out main.c, $(wildcard *.c))
OBJ	:= $(SRC:.c=.o) $(wildcard ../src/*.o)
TOP	:= main.c

all:	run bench
run:	bench
	./bench

clean:
	rm *.o bench

bench: $(TOP) $(OBJ)
	gcc -I../src/ $^ -Wall -lm -O3 -o $@

%.o: %.c
	gcc -I../src/ $^ -Wall -O3 -c -o $@
