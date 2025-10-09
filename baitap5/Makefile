
CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -O2

SRCS = ex1_sigint_count.c ex2_sigalrm_timer.c ex3_parent_child_signal.c ex4_select_signal_input.c
BINS = ex1 ex2 ex3 ex4

all: $(BINS)

ex1: ex1_sigint_count.c
	$(CC) $(CFLAGS) -o $@ ex1_sigint_count.c

ex2: ex2_sigalrm_timer.c
	$(CC) $(CFLAGS) -o $@ ex2_sigalrm_timer.c

ex3: ex3_parent_child_signal.c
	$(CC) $(CFLAGS) -o $@ ex3_parent_child_signal.c

ex4: ex4_select_signal_input.c
	$(CC) $(CFLAGS) -o $@ ex4_select_signal_input.c

.PHONY: clean run1 run2 run3 run4

clean:
	rm -f $(BINS) *.o

run1: ex1
	./ex1

run2: ex2
	./ex2

run3: ex3
	./ex3

run4: ex4
	./ex4
