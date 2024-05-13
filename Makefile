.PHONY:	clean bench run src all

all:	run bench
run:	bench
	@make -C bench run

src:
	@make -C src all

bench:	src
	@make -C bench all

clean:
	@make -C bench clean
	@make -C src clean

%.o: %.c
	gcc $^ -Wall -O3 -c -o $@
