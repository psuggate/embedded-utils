.PHONY:	clean bench run all

SRC	:= $(wildcard *.c)
OBJ	:= $(SRC:.c=.o)

all:	$(OBJ)

clean:
	rm *.o

%.o: %.c
	gcc $^ -Wall -O3 -c -o $@
