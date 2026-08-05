# HTTP Server Project Notes

## Project Goal

Build an HTTP server in C from first principles to learn:

* Socket programming
* Networking fundamentals
* HTTP protocol
* File I/O
* Concurrency
* Systems programming concepts

Current status: **Persistent HTTP server with static file serving, dynamic routing, CSS support and 404 handling.**

---

# M1 - Socket Fundamentals

## Core Functions

### socket()

Creates a socket and returns a file descriptor.

```c
int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
```

Think:

```text
Socket == endpoint for communication
```

### bind()

Associates socket with an IP address and port.

```c
bind(sock_fd, ...);
```

Without bind, the OS doesn't know which port the server should listen on.

### listen()

Marks socket as a listening socket.

```c
listen(sock_fd, backlog);
```

Creates a queue for incoming connections.

### accept()

Accepts a connection from the queue.

```c
int client_fd = accept(...);
```

Important:

```text
sock_fd    -> listening socket
client_fd  -> connected client socket
```

accept() returns a NEW file descriptor.

This was one of the most important realizations during the project.

---

# Network Interface Notes

Machine can have multiple addresses:

```text
127.0.0.1      -> loopback
192.168.x.x    -> local network
public IP      -> internet
```

Server can bind to:

```text
127.0.0.1      -> localhost only
0.0.0.0        -> all interfaces
```

---

# M2 - Client/Server Communication

## read()

```c
read(client_fd, buffer, sizeof(buffer));
```

Blocks until data arrives.

Returns number of bytes read.

Important:

```c
ssize_t bytes_read
```

must always be checked.

---

## send()

```c
send(client_fd, buffer, len, 0);
```

Transmits bytes over the socket.

---

## close()

```c
close(client_fd);
```

Closes descriptor.

OS reclaims resources.

---

# M3 - HTTP

## Typical Request

```http
GET /about HTTP/1.1
Host: localhost:8080
User-Agent: Firefox
```

Important fields:

```text
Method
Route
Protocol Version
```

---

## Parsing

Current approach:

```c
sscanf(buffer,"%s %s",method,route);
```

Extracts:

```text
GET
/about
```

---

## Typical Response

```http
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 123

<html>...</html>
```

---

## Content-Length

Critical header.

Browser needs to know how many bytes belong to the response body.

Discovered bug:

```http
Content-Length: <html_length>
```

Sent literal text instead of actual length.

Fixed using:

```c
sprintf(...)
```

---

# M4 - Static File Server

## Route Mapping

Current style:

```text
/               -> static/index.html
/about          -> static/about.html
```

Later extended for assets:

```text
/style.css      -> static/style.css
```

---

## File Serving

```c
FILE *fptr = fopen(filepath,"r");
```

Read file:

```c
fread(...)
```

Send file contents back to browser.

---

## 404 Handling

If:

```c
fopen(...) == NULL
```

Serve:

```text
static/error404.html
```

and return:

```http
HTTP/1.1 404 NOT FOUND
```

---

# Persistent Server

Original:

```c
accept()
handle()
return 0;
```

Server died after one request.

Current:

```c
while(1)
{
    accept();
    handle_request();
    close(client_fd);
}
```

Server runs indefinitely.

---

# CSS Support

Browser now requests multiple resources:

```text
GET /
GET /style.css
```

Server serves both.

This was the transition from:

```text
Serving documents
```

to:

```text
Serving resources
```

---

# MIME Types

Need correct Content-Type.

Examples:

```text
.html -> text/html
.css  -> text/css
.jpg  -> image/jpeg
.png  -> image/png
```

Extension extraction:

```c
char *ext = strrchr(filepath,'.');
```

Use last '.' not first.

---

# Major Bugs Encountered

## Bug #1 - HTTPS vs HTTP

Symptom:

```text
curl works
Firefox fails
```

Cause:

```text
Browser attempted HTTPS
Server only understood HTTP
```

Result:

```text
Garbage bytes
TLS handshake data
```

Lesson:

```text
HTTPS != HTTP
```

---

## Bug #2 - Literal Content-Length

Sent:

```http
Content-Length: <html_length>
```

instead of actual number.

---

## Bug #3 - Pointer vs String

Had:

```c
const char *code = "200 OK";
```

Accidentally used:

```c
*code
```

instead of:

```c
code
```

Result:

```http
HTTP/1.1 2
```

instead of:

```http
HTTP/1.1 200 OK
```

Lesson:

```text
code  -> string
*code -> first character
```

---

## Bug #4 - strcmp()

Forgot:

```c
strcmp(a,b) == 0
```

means strings are equal.

---

## Bug #5 - Missing Newline in printf

```c
printf("server still up");
```

Output buffered.

Appeared as if print never executed.

Fixed using:

```c
printf("server still up\n");
```

---

# Systems Programming Lessons

## File Descriptor

A socket is represented by an integer.

```c
sock_fd
client_fd
```

are file descriptors.

Sockets and files use similar OS abstractions.

---

## Listening Socket vs Client Socket

```text
sock_fd
```

Accepts connections.

```text
client_fd
```

Communicates with a specific client.

Never confuse the two.

---

## Resources

Always think:

```text
open
use
close
```

Examples:

```text
FILE*
socket
pipe
```

---

# Upcoming Milestones

## M5 - Binary Assets

Serve:

```text
.jpg
.png
.ico
```

Goal:

Understand why:

```c
strlen()
```

is unsafe for binary data.

Need to use:

```c
bytes_read
```

instead.

---

## M6 - Concurrency

Explore:

```c
pthread_create()
```

Goals:

* Multiple clients simultaneously
* Thread lifecycle
* Synchronization
* Race conditions

---

## Long-Term Goals

* Thread pool
* Non-blocking sockets
* epoll
* Logging
* HTTPS
* Configuration files
* Deployment on VPS


# M5 - Binary-Safe File Serving & Streaming

## Goal

Evolve the server from a text-only HTTP server into a generic file server capable of serving binary assets such as images.

---

## Features Added

- JPEG/PNG image serving
- Binary-safe file transmission
- Streaming files in chunks
- File size discovery using `stat()`
- Dynamic MIME type handling

---

# Major Concepts Learned

## 1. Strings vs Raw Bytes

Initially, file contents were treated as C strings.

```c
send(client_fd, buffer, strlen(buffer), 0);
```

This works for HTML because HTML is text.

It completely breaks for binary files because binary data may contain `'\0'` bytes anywhere.

### Correct Mental Model

A web server does **not** send strings.

A web server sends **bytes**.

Correct approach:

```c
send(client_fd, buffer, bytes_read, 0);
```

where `bytes_read` comes directly from `fread()`.

---

## 2. Null Terminators Are a String Concept

Previously:

```c
buffer[bytes_read] = '\0';
```

This was only necessary because string functions such as

- strlen()
- printf("%s")
- strcpy()

expect null-terminated strings.

For binary files this is unnecessary.

The server should think in terms of

```
Read N bytes
↓

Send N bytes
```

instead of

```
Read String
↓

strlen()
↓

Send String
```

---

## 3. Binary Files Can Be Larger Than The Buffer

Initial implementation:

```
fread()
↓

send()
```

worked only if

```
file_size <= buffer_size
```

Large JPEGs exposed this limitation.

Example:

```
Image Size : 39 KB
Buffer Size: 8 KB
```

Only ~20% of the image was transmitted.

This naturally motivated streaming.

---

## 4. Streaming Files

Instead of reading the entire file:

```
Read entire file
↓

Send
```

the server now performs

```
Read Chunk
↓

Send Chunk
↓

Repeat
```

Implementation:

```c
while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0)
{
    send(client_fd, buffer, bytes_read, 0);
}
```

This removes the practical file size limitation.

---

## 5. Operator Precedence Bug

Bug:

```c
while(byte_read = fread(...) > 0)
```

Actual evaluation:

```c
byte_read = (fread(...) > 0)
```

Result:

```
byte_read = 0 or 1
```

Only one byte was being sent.

Correct version:

```c
while ((byte_read = fread(...)) > 0)
```

A classic C precedence bug.

---

## 6. HTTP Response Structure

Initially the response header was sent inside the streaming loop.

Result:

```
Header
Body
Header
Body
Header
Body
```

Correct HTTP response:

```
Header

Body...
```

Headers must be transmitted exactly once before the response body.

---

## 7. Content-Length

Important realization:

`Content-Length` is

**NOT**

```
current_chunk_size
```

It is

```
entire_response_size
```

The browser must know the total response size before receiving the first body byte.

---

## 8. Using stat()

Instead of reading the file twice to determine its length, the filesystem already stores metadata.

Using

```c
stat(filepath, &file_stat);
```

allows retrieval of

```
file_stat.st_size
```

which becomes the HTTP Content-Length.

This is both efficient and closer to how real servers operate.

---

# Debugging Journey

## Image Only Partially Displayed

Observation:

Only the cat's forehead was visible.

Initial suspicion:

- CSS issue
- HTML layout

Actual cause:

Image size exceeded buffer capacity.

Only the first 8192 bytes were being transmitted.

---

## CSS Appeared Correct But Image Still Broken

Important debugging lesson:

Not every rendering issue is a frontend issue.

The browser can successfully decode **partial** JPEG data, producing misleading visual artifacts.

Always verify:

- file size
- bytes sent
- buffer size
- Content-Length

before assuming CSS is responsible.

---

## Realization

Today's biggest conceptual shift:

The server is no longer an HTML server.

It is now a generic byte-streaming server.

HTML, CSS, JPEG, PNG and future file types are all simply sequences of bytes accompanied by the appropriate MIME type.

---

# Interview Talking Points

### Interesting Bug

> While adding image support, only the top portion of JPEGs rendered correctly.

Root cause:

The server read the file only once into an 8 KB buffer and transmitted only those bytes.

Resolution:

Implemented chunked file streaming using `fread()` in a loop and corrected HTTP `Content-Length` using filesystem metadata from `stat()`.

---

### Key Learning

> HTTP servers should not think in terms of strings.

They should think in terms of byte streams.

Understanding this distinction made the server capable of serving arbitrary binary assets instead of only text files.

### Debugging Lesson: Don't Blame the New Feature Too Quickly

aug 5 

issue: tried implimenting pthreads which broke image rendering

Symptoms:
- Images failed after introducing pthreads.
- curl worked, Firefox didn't.
- Removing pthreads fixed the issue.

Investigation:
- Verified image streaming worked independently using curl.
- Eliminated HTTP headers, MIME types, and fread/send loops.
- Isolated the regression to the pthread refactor.
- Found that the accept loop exited after 3 requests because of `i < 3`.

Lesson:
The root cause wasn't pthreads—it was test scaffolding (`i < 3`) left in the main server loop. When debugging, isolate the smallest change that introduced the regression instead of assuming the new technology is at fault.