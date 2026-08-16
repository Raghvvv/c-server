CC := gcc
CFLAGS := -Wall -Wextra -pthread

SERVER_SOURCES := src/main.c src/worker.c src/server1.c src/http.c src/router.c src/response.c src/queue.c
SERVER := src/server

.PHONY: all clean

all: $(SERVER)

$(SERVER): $(SERVER_SOURCES)
	$(CC) $(CFLAGS) $(SERVER_SOURCES) -o $@

clean:
	rm -f $(SERVER)
