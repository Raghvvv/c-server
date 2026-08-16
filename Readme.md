# HTTP Server in C

A lightweight HTTP server built from scratch in C using POSIX sockets and pthreads.

The server accepts TCP connections, parses basic HTTP requests, serves static files, and handles multiple clients concurrently using a fixed-size thread pool.

## Features

* TCP server using POSIX sockets
* Basic HTTP request parsing
* Chunked file reads
* Fixed-size pthread thread pool
* Thread-safe producer-consumer queue
* Mutexes and condition variables

## Architecture

```text
              accept()
                  |
                  v
          +---------------+
          |  Request Queue |
          +---------------+
             ^         |
             |         v
          Main      Worker Threads
         Thread      W1 W2 W3 W4
                       |
                       v
                 serve_client()
                       |
                 Parse → Route
                       |
                    File I/O
                       |
                    Response
```

The main thread accepts connections and places socket descriptors into a shared queue. Worker threads consume connections and handle requests.

The queue uses a mutex for mutual exclusion and condition variables to avoid busy-waiting when no work is available.

## Project Structure

```text
.
├── src/
│   ├── main.c
│   ├── server1.c
│   ├── http.c
│   ├── router.c
│   ├── response.c
│   ├── worker.c
│   ├── queue.c
│   └── *.h
├── static/
├── Makefile
└── README.md
```

## Build & Run

From the project root:

```bash
make
./src/server
```

The server listens on port `8080`.

Open:

```text
http://localhost:8080/
```

Or test with:

```bash
curl http://localhost:8080/
curl http://localhost:8080/nonexistent
```

## Concurrency Test

```bash
for i in {1..50}; do
    curl -s http://localhost:8080/ > /dev/null &
done
wait
```

## Concepts

This project was used to explore:

* POSIX sockets and file descriptors
* HTTP over TCP
* Blocking I/O
* File and binary I/O
* pthreads and concurrent request handling
* Thread lifecycle and argument ownership
* Producer-consumer synchronization
* Mutexes and critical sections
* Condition variables
* Thread pools
* C source/header organization
* Memory debugging with AddressSanitizer and Valgrind

## Testing

The server was tested with AddressSanitizer and Valgrind.

Valgrind reported:

```text
definitely lost: 0 bytes
indirectly lost: 0 bytes
```

## Limitations

This is an educational HTTP server focused on static content and basic HTTP handling. It is not intended to be a production HTTP implementation.
