CC = gcc
CFLAGS = -Wall -g
TARGETS = producer consumer

all: $(TARGETS)

producer: posix_shm_producer.c
	$(CC) $(CFLAGS) -o producer posix_shm_producer.c -lrt

consumer: posix_shm_consumer.c
	$(CC) $(CFLAGS) -o consumer posix_shm_consumer.c -lrt

run:
	@echo "=== Chạy Producer ==="
	./producer
	@echo ""
	@echo "=== Chạy Consumer ==="
	./consumer

clean:
	rm -f $(TARGETS)
