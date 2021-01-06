## Header
```
test machine: csel-vole-36
date: November 21st, 2020
name: Ryan Mower, Zach Siew, Isaac Lee
x500: mower023, leex8934, siew0053
groupID: 31
```

## How to compile program
We additionally added the math library to the makefile for compilation. Other than that, the Makefile is the same as the original.
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
We attempted both Extra Credit A, the dynamic worker thread pool, and Extra Credit B, caching.

## Implementation of Dynamic Worker Thread Pool Policy [Extra Credit A]
Our policy to change the number of active threads is based off the ratio of pending requests to the queue lengh. We take this ratio and we activate the number of threads that will produce the same ratio with active workers to max_worker_threads (supplied by command line). Therefore, if our queue is empty, we keep one active worker. However, if our queue is full, we will spawn up to the max_worker_threads, trying to free up the queue.

## Implementation of Caching [Extra Credit B]
We use an LRU (last recently used) policy for replacing requests in our cache. So, while storing a request in the queue our program will also keep track of the age of that request. This is important to know, because when the cache is full, but we need to add a new request to the cache, we find the oldest request and replace it with the new request.

## Any Assumptions
Vim was the primary text editor for this project. If tabs/spacing seems off, try setting tabstop and shiftwidth to 4. Then the code should allign properly.

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
  dispatcher
  worker
  extra credit A (Dynamic Pooling)
  extra credit B (Caching)
  README
  synchronization
  condition variables
  utilities

Zach:
  ring buffer implementation
  dispatcher
  worker
  extra credit A (Dynamic Pooling)
  
Isaac:
  dispatcher
  worker
  extra credit B (Caching)
  Created plan for extra credits
  README
```
