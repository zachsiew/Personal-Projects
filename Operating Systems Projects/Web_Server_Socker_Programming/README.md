## Header
```
test machine: csel-vole-36
date: Dec 9th, 2020
name: Ryan Mower, Zach Siew, Isaac Lee
x500: mower023, leex8934, siew0053
groupID: 31
```

## How to compile program
```
make clean
make
```

To run the program.  
```
./web_server <port> <path_to_testing>/testing <num_dispatch> <num_worker> <dynamic_flag> <queue_len> <cache_entries>
```

## How our program works
The purpose of our program is to act as a web server that can handle file requests of types HTML, GIF, JPEG and TXT. Our program uses two different types of POSIX threads to handle server requests. The first type are the dispatcher threads, which check for and queue up any incoming requests to the server. The second type are the worker threads, which process requests as they appear in the queue and return the data requested back to the client. When processing a request, the worker thread will first try to check if the data requested is already stored in the cache. If it is not found there, the data is pulled from the disk and is added to the cache, replacing the oldest request still being stored the cache. More details about how our cache was implemented are included below. Once processed, the number of bytes data (or an error should one occur) is returned by a request is recorded in the logfile along with other information to identify the work being done by different worker threads. The number of worker threads also changes dynamically with server load (assuming the dynamic flag is set to 1). Explanation of our implementation of the dynamic pool is described below.

## Extra Credit
Not Attempted at the moment.


## Any Assumptions

1. The maximum number of dispatcher threads will be 100.
2. The maximum number of worker threads will be 100.
3. The maximum length of the request queue will be 100.
4. The maximum size of the cache will be 100 entries.
5. The maximum length of a filename will be 1024.
6. Any HTTP request for a filename containing two consecutive periods or two consecutive slashes (".." or "//") will automatically be detected as a bad request by the compiled code for security.
7. The number of worker threads specified by the client when the server is run will be the maximum number of worker threads running at a time when the dynamic thread pool is being used.  

## Contributions by each member
We all worked on most parts of the project together so that we could all learn the different aspects of the project.
```
Ryan:
  README.md
  return_result
  init
  accept_connection
  get_request
  IntermiReport

Zach:
  init
  return_error
  init
  return_result
  accept_connection
  get_request
  
Isaac: 
  init
  return_error 
  return_result
  accept_connection
  get_request
```
