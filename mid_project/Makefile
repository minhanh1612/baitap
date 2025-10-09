
CC=gcc
CFLAGS=-Wall -Wextra -pthread
TARGET=chat

all: $(TARGET)

$(TARGET): chat.c
	$(CC) $(CFLAGS) -o $(TARGET) chat.c

clean:
	rm -f $(TARGET)
