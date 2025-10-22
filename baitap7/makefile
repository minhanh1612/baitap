CC = gcc
CFLAGS = -Wall -lrt

all: task_worker task_dispatcher

task_worker: task_worker.c
	$(CC) task_worker.c -o task_worker $(CFLAGS)

task_dispatcher: task_dispatcher.c
	$(CC) task_dispatcher.c -o task_dispatcher $(CFLAGS)

clean:
	rm -f task_worker task_dispatcher
