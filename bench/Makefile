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
	gcc $^ -Wall -lm -O3 -o $@

%.o: %.c
	gcc $^ -Wall -O3 -c -o $@
