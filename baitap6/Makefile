
CC = gcc
CFLAGS = -Wall

TARGETS = ipv4_stream_server ipv4_stream_client \
          ipv4_dgram_server ipv4_dgram_client \
          unix_stream_server unix_stream_client \
          unix_dgram_server unix_dgram_client

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<


clean:
	rm -f $(TARGETS) /tmp/demo_socket /tmp/server_dgram_socket /tmp/client_dgram_socket

.PHONY: all clean