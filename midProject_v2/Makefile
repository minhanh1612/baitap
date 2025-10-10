CC=gcc
CFLAGS=-Wall -Iinclude -pthread
SRCS=src/main.c src/server.c src/client.c src/message.c src/utils.c
OBJS=$(SRCS:.c=.o)
TARGET=chat

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)
