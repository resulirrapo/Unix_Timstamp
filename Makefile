CFLAGS=-Wall -Werror

.PHONY: clean run-bitpacking run-planner

run-bitpacking: bitpacking
	./bitpacking

run-planner: planner
	./planner

bitpacking: bitpacking.c timelib.c
	gcc bitpacking.c timelib.c $(CFLAGS) -o bitpacking

planner: planner.c timelib.c
	gcc planner.c timelib.c $(CFLAGS) -o planner

clean:
	rm -f bitpacking planner
