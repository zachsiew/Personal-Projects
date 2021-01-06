#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include "util.h"
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#define MAX_THREADS 100
#define MAX_queue_len 100
#define MAX_CE 100
#define INVALID -1
#define BUFF_SIZE 1024
#define UPDATE_FREQ 1 // Time in seconds for dynamic pool to update
/*
  THE CODE STRUCTURE GIVEN BELOW IS JUST A SUGGESTION. FEEL FREE TO MODIFY AS NEEDED
*/

// structs:
typedef struct request_queue {
   int fd;
   char *request;
} request_t;

typedef struct cache_entry {
    int len;
    char *request;
    char *content;
} cache_entry_t;

// Struct for woker thread arguments
typedef struct worker_args {
    int id;
    int queue_length;
} worker_args_t;

// Args for dynamic pooling
typedef struct pool_args{
    int numWorkerThreads;
    int queue_length;
} pool_args_t;

// Request Queue Buffer that stors the requests and their file descripters
// Dispatch and Worker operates on to remove and insert requests
struct request_queue ringBuffer[MAX_queue_len];

// File for logging requests
FILE *logFile;

// Done flag
int doneFlag = 1;  // (Not done)

// Indexes tracking where to insert/remove in ringBuffer
int insert_idx = 0;
int remove_idx = 0;
int pending_requests = 0;

// Locks
pthread_mutex_t logFileLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ringBufferLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cacheLock = PTHREAD_MUTEX_INITIALIZER;

// Condition Variables
pthread_cond_t waitForEmptySlot = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitForDataSlot= PTHREAD_COND_INITIALIZER;

// Globals for dynamic pooling
int flags[MAX_THREADS];
int active_workers = 0;
int wanted_workers = 0;
struct worker_args wargs[MAX_THREADS];

// Cache
struct cache_entry cache[MAX_CE];
int cache_replacement[MAX_CE];
int cache_len;



/* ************************ Cache Code [Extra Credit B] **************************/

// Updates cache age if there was a hit.
// Resets hit's index age and increments others
void updateCacheAgeOnHIT(int idx){
	for (int i = 0; i < cache_len; i++){
		cache_replacement[i]++;
	}
	cache_replacement[idx] = 0;							// Resets idx of hit age
}

// Gets the cache index on hit, otherwise returns -1 on miss
int getCacheIndex(char *request){
  /// return the index if the request is present in the cache
  for(int i =  0; i < cache_len; i++){
    if (cache[i].request != NULL){ 						// Checks so doesn't string compare against NULL
		  if (strcmp(cache[i].request, request) == 0){
			  updateCacheAgeOnHIT(i);
			  return i;  								// Index corresponding to where request is inside the cache
		  }
	  }
  }
  return -1; // Cache MISS

}

// Function to add the request and its file content into the cache
void addIntoCache(char *mybuf, char *memory , int memory_size){
  // It should add the request at an index according to the cache replacement policy
  // Make sure to allocate/free memory when adding or replacing cache entries

  // Find where to insert
  pthread_mutex_lock(&cacheLock);		// Locks cache
  int where_to_add = 0;
  int age = cache_replacement[0];
  for (int i = 0; i < cache_len; i++){	// Only look at valid part of cache
      if (cache_replacement[i] >= age){ // Finds oldest element in cache
		      age = cache_replacement[i];
		      where_to_add = i;
	    }
  }

  // Clear old entry
  if (cache[where_to_add].request != NULL){
	  free(cache[where_to_add].request);
  }
  if (cache[where_to_add].content != NULL){
	  free(cache[where_to_add].content);
  }

  // Adding entry
  int string_length = strlen(mybuf);
  cache[where_to_add].len = memory_size;
  cache[where_to_add].request = (char *) malloc(string_length);		// Allocate memory for data
  cache[where_to_add].content = (char *) malloc(memory_size);
  strcpy(cache[where_to_add].request, mybuf);						// Cpy request into cache
  memcpy(cache[where_to_add].content, memory, memory_size); 		// Copies content into cache
  // Updates ages inside cache
  for (int i = 0; i < cache_len; i++){
  	cache_replacement[i]++;
  }
  cache_replacement[where_to_add] = 0;								// Resets age of newly added entry
  pthread_mutex_unlock(&cacheLock);									// Unlocks cache
  return;
}

// Clears all memroy for the cache
// Used before program exits
void deleteCache(){
  // De-allocate/free the cache memory
  for (int i = 0; i < cache_len; i++){
	free(cache[i].request); // Free request
	free(cache[i].content); // Free content
  }
}

// Function to initialize the cache
// Prepares cache for later use
void initCache(){
  // Allocating memory and initializing the cache array
  for (int i = 0; i < MAX_CE; i++){	// Initialize entire cache
	cache[i].len = 0;					// Wont use area after cache_len
	cache[i].request = NULL;
	cache[i].content = NULL;
  }
  for (int i = 0; i < MAX_CE; i++){    // Zero out cache_replacement
	cache_replacement[i] = 0;
  }
}

/**********************************************************************************/

/* ************************************ Utilities ********************************/
// Function to get the content type from the request
char* getContentType(char * mybuf) {
  // Should return the content type based on the file type in the request
  // (See Section 5 in Project description for more details)

  // This idea for grabing the file extension was taken
  // from: https://stackoverflow.com/questions/5309471/getting-file-extension-in-c
  char *extension = strrchr(mybuf, '.');
  if (extension == NULL){
    printf("%s did not have a valid extension.\n",mybuf);
    exit(0);
  }
  // Finding extension
  if(strcmp(".html", extension) == 0){
    return "text/html";
  }
  else if (strcmp(".jpg", extension) == 0){
    return "image/jpeg";
  }
  else if (strcmp(".gif", extension) == 0){
    return "image/gif";
  }
  else{
    return "text/plain";
  }
}

// Function to open and read the file from the disk into the memory
char * readFromDisk(char *fileName, int *bytes) {
    // Open and read the contents of file given the request
	int fd;
	struct stat fileMeta;		// Used to collect file size
	char c[1];
	int pos = 0;
	char *data = NULL;			// Pointer to data loaded into memory

	// Error checks and opens file for reading
	if ((fd= open(fileName, O_RDONLY)) == -1){
	    return NULL;
	}

	// Error checks getting metadata on filename
	if(fstat(fd, &fileMeta) == -1){
		printf("Failed to get meta data on file: %s\n", fileName);
		exit(0);
	}

	// Stores file size in *bytes (numBytes from caller)
	*bytes = (int) fileMeta.st_size;
	// Creates space to store file in memoryize;
	data = malloc(*bytes);

	// Reads file into memory (data)
	while(read(fd, c, 1) > 0){
		data[pos] = c[0];
		pos++;
	}
	close(fd);		// Closes file are returns
	return data;	// pointer to data
}
/**********************************************************************************/
// Checks if ringBuffer is full
int isFull(int i_idx, int r_idx, int q_len){
	if (((i_idx + 1) % q_len) == r_idx){
		return 1;	// Ring buffer full
	}
	else{
		return 0;	// Ring buffer not null
	}
}

// Inserts a request into ringBuffer
void insert_request(int fd, char * fileName, int qlen){
  // Inserts request
  pthread_mutex_lock(&ringBufferLock);
  while(isFull(insert_idx, remove_idx, qlen)){  
  	pthread_cond_wait(&waitForEmptySlot, &ringBufferLock); // Waits for free spot in ringSpot
  }
  // Inserts into ringBuffer
  pending_requests++;
  ringBuffer[insert_idx].fd = fd;
  ringBuffer[insert_idx].request = fileName;
  insert_idx = (insert_idx + 1) % qlen;
  pthread_cond_signal(&waitForDataSlot);
  // send signar to waitForDataSlot
  pthread_mutex_unlock(&ringBufferLock);
  return;
}

// Forever function that constantly retrieves requests 
// from the client and inserts them into the queue
// for later processing.
void * dispatch(void *arg) {
  int qlen = *((int *) arg);
  while (1) {

    // Accept client connection
    int fd; //connection
    if((fd = accept_connection()) < 0){
      continue; 	// Back connection, ignore
    }

    // Get request from the client
    char * fileName = (char *) malloc(BUFF_SIZE); 
    if(get_request(fd, fileName) != 0) {
      //Ignoring bad connection
      continue;
    }

    // Add the request into the queue
    insert_request(fd, fileName, qlen);
   }
   return NULL;
}

/**********************************************************************************/


// Removes request from ringBuffer
// Thread safe
void remove_request(int *fd, char **fileName, int qlen){
  pthread_mutex_lock(&ringBufferLock);
  while(insert_idx == remove_idx){
  	pthread_cond_wait(&waitForDataSlot, &ringBufferLock); // Waits for data inside ringBuffer if it is empty
  }
  //Removes request from ringBuffer
  pending_requests--;
  *fd = ringBuffer[remove_idx].fd;
  *fileName = ringBuffer[remove_idx].request;
  remove_idx = (remove_idx + 1) % qlen;		// Resets index of where to remove from next
  pthread_cond_signal(&waitForEmptySlot);	// Unlocks and signals
  pthread_mutex_unlock(&ringBufferLock);

  return;
}

// Logs and prints data to screen
// Used to communicate info such as HIT/MISS, file descriptor and name, size and other
// important information to the user
void logInfo(int id, int requestsProcessed, int fd, char *fileName, int numBytes, char *h_m){
	// Checks to see if data was read successfully
	pthread_mutex_lock(&logFileLock); // Locks logFile
	if (numBytes == -1){
		// Log the request into the file and terminal
		printf("[%d][%d][%d][%s][Requested file not found.][%s]\n", id, requestsProcessed, fd, fileName, h_m);
		fprintf(logFile,"[%d][%d][%d][%s][Requested file not found.][%s]\n", id, requestsProcessed, fd, fileName, h_m);
	}
	else{
		// Log the request inot the file and terminal
		printf("[%d][%d][%d][%s][%d][%s]\n", id, requestsProcessed, fd, fileName, numBytes, h_m);
		fprintf(logFile, "[%d][%d][%d][%s][%d][%s]\n", id, requestsProcessed, fd, fileName, numBytes, h_m);

	}
	pthread_mutex_unlock(&logFileLock); // Unlocks logFile

}


// Function to retrieve the request from the queue, 
// process it and then return the result to the client
void * worker(void *arg) {
  struct worker_args wargs = *((worker_args_t *) arg);
  int id = wargs.id;
  int qlen = wargs.queue_length;
  int requestsProcessed = 0;
  int numBytes = 0;		// File Size
  int fd;
  char *contentTyp = NULL;
  char *data = NULL;    // Pointer to file loaded in memory
  char *fileName = NULL;
  int cache_idx = 0;
  //printf("Inside worker, got values.\n");
  while (1) {
    // Get the request from the queue
    fileName = NULL;
	if(flags[id]){
		remove_request(&fd, &fileName, qlen);
		requestsProcessed++;
		char *tmp = strdup(fileName);
		sprintf(fileName, "%s%s", ".", tmp);
		// Get the data from the disk or the cache (extra credit B)
		if((cache_idx = getCacheIndex(fileName)) == -1){
			// read from disk
			if((data = readFromDisk(fileName, &numBytes)) == NULL){
				// Thread safe loggin info
				logInfo(id, requestsProcessed, fd, fileName, -1, "MISS");

				// Returns an error
				if(return_error(fd, "") != 0){
					printf("Error in returning an error fd: %d", fd);
				}
			}
			else{
				// Thread safe loggin info
				logInfo(id, requestsProcessed, fd, fileName, numBytes, "MISS");
				contentTyp = getContentType(fileName);
				if(return_result(fd, contentTyp, data, numBytes) !=0){  // Ensures return_result succeeds
					printf("Error in sending result to fd: %d.", fd);
				}

				// Add request and data to data
				addIntoCache(fileName, data, numBytes);
				free(data); // Frees file loaded into memory
			}

		}
		else{
			// Found data in cache
			logInfo(id, requestsProcessed, fd, fileName, cache[cache_idx].len, "HIT");
			contentTyp = getContentType(fileName);
			if(return_result(fd, contentTyp, cache[cache_idx].content, cache[cache_idx].len) != 0){
				printf("Error in sending result to fd: %d.\n", fd);
			}

		}
		free(fileName);	// Frees the filename
	  } else{
		return NULL; // Worker turned off
	  }
  }

  return NULL;
}

/* ******************** Dynamic Pool Code  [Extra Credit A] **********************/
// Extra Credit: This function implements the policy to change the worker thread pool dynamically
// depending on the number of requests

// Depending on the magnitude of UPDATE_FREQ, this function will constatnly update
// the current number of active threads. It does this by taking the the ratio of 
// how full the queue is and spawn/despawn threads to equal the same ratio of active
// threads to max threads (passed in on command line).
void * dynamic_pool_size_update(void *arg) {
  struct pool_args pargs = *((pool_args_t *) arg);
  int qlen = pargs.queue_length;
  int numWorkers = pargs.numWorkerThreads;
  double ratio = 0;
  int worker_diff = 0;
  pthread_t thread;

  while(1) {
    // Run at regular intervals
    // Increase / decrease dynamically based on your policy
    ratio = (double) pending_requests / qlen;
    wanted_workers = floor((double) (numWorkers * ratio));

    // Case when pending requests is 0
    if ( wanted_workers == 0){
		wanted_workers = 1;
    }
	// Spawn or delete threads here
	worker_diff =  wanted_workers - active_workers;
	if (worker_diff >= 0){
  		pthread_t workThreads[worker_diff]; for(int i = 0; i < worker_diff; i++){
			flags[active_workers] = 1;
		  	wargs[active_workers].id = active_workers;
		  	wargs[active_workers].queue_length = qlen;
		  	// Creates worker threads
		  	if((thread = pthread_create(&workThreads[i], NULL, (void *) worker, &wargs[i])) != 0){ // Ensures successful thread Creatiion
		  	  printf("Failed to create worker thread.");
		  	  exit(1);
		  	}
		  	else{
		  	  // Detaches worker threads
		  	  if(pthread_detach(workThreads[i]) !=0) {	// Ensures detaches thread successfully
		  		printf("Failed to detach worker thread: %ld.\n", thread);
		  		exit(1);
		  	  }
		  	}
			active_workers++;
		}
		if (worker_diff > 0){
			printf("Created %d threads because pending request ratio increased to %lf.\n", worker_diff, ratio);
		}
	}
	else{
		// Despawn workers
		for(int i = 0; i < MAX_THREADS; i++){
			if (i < (active_workers + worker_diff)){
				flags[i] = 1;
			}
			else{
				flags[i] = 0;
			}
		}
		active_workers = active_workers + worker_diff; // Adjusts num of active_workers
		printf("Removed %d threads because pending request ratio decreased to %lf.\n", (-1 * worker_diff), ratio);
	}
	


	sleep(UPDATE_FREQ); // Slow down aggressive pool oscillation
  }
}
/**********************************************************************************/

// Signal handler for ctrl^c that sets the doneFlag
// Used to terminate program laster
static void closeServer(int signo) {
	doneFlag = 0; // Ctrl^c hit, program is done
}


// --------  MAIN  --------
int main(int argc, char **argv) {

  // Error check on number of arguments
  if(argc != 8){
    printf("usage: %s port path num_dispatcher num_workers dynamic_flag queue_length cache_size\n", argv[0]);
    return -1;
  }

  // Get the input args
  int portNum = strtol(argv[1], NULL, 10);
  char *inputFile = argv[2];
  int numDispatcher = strtol(argv[3], NULL, 10);
  int numWorkers = strtol(argv[4], NULL, 10);
  int dynamicFlag = strtol(argv[5], NULL, 10);
  int qlen = strtol(argv[6], NULL, 10);
  int cacheEntries = strtol(argv[7], NULL, 10);


  // Change SIGINT action for graceful termination
  struct sigaction act;
  act.sa_handler = closeServer;
  act.sa_flags = 0;
  if ((sigemptyset(&act.sa_mask) == -1) ||
    (sigaction(SIGINT, &act, NULL) == -1)) {
      perror("Failed to set SIGINT handler");
      return -1;
  }

  // Open log file for writing
  if((logFile = fopen("web_server_log", "w")) == NULL){
    printf("Failed to open log file.");
    exit(1);
  }

  // Change the current working directory to server root directory
   if(chdir(inputFile) == -1){
     printf("Failed to change directory to %s\n", inputFile);
     exit(1);
   }

  // Initialize cache (extra credit B)
  initCache();
  cache_len = cacheEntries;
  // Start the server
  init(portNum);

  // Create dispatcher and worker threads (all threads should be detachable)
  pthread_t thread;
  pthread_t dispThreads[numDispatcher];
  pthread_t workThreads[numWorkers];

  // Creates dispatcher threads
  for(int i = 0; i < numDispatcher; i++){
    if((thread = pthread_create(&dispThreads[i], NULL, (void *) dispatch, &qlen)) != 0){ // Ensures successful thread creation
      printf("Failed to create dispatch thread.");
      exit(1);
    }
    else{
	  // Detaches dispatchers threads		IS THIS OKAY??
      if(pthread_detach(dispThreads[i]) != 0){	// Ensures detaches threads successfully
        printf("Failed to detach dispatcher thread: %ld.\n", thread);
        exit(0);
      }
    }
  }
  // Array for worker ids, and the queue length
  for(int i = 0; i < numWorkers; i++){
  	wargs[i].id = i;
	wargs[i].queue_length = qlen;
	// Creates worker threads
    if((thread = pthread_create(&workThreads[i], NULL, (void *) worker, &wargs[i])) != 0){ // Ensures successful thread Creatiion
      printf("Failed to create worker thread.");
      exit(1);
    }
    else{
      // Detaches worker threads
      if(pthread_detach(workThreads[i]) !=0) {	// Ensures detaches thread successfully
        printf("Failed to detach worker thread: %ld.\n", thread);
		exit(0);
      }
    }
  }

  // Create dynamic pool manager thread (extra credit A)
  // Initialize flags to all 0's
  for(int i = 0; i < numWorkers; i++){
  	flags[i] = 0;
  }

  struct pool_args pargs;
  pargs.numWorkerThreads = numWorkers;
  pargs.queue_length = qlen;

  // Creating dynamic pooling thread
  if(dynamicFlag){
  	pthread_t dynamic_thread;
	if((thread = pthread_create(&dynamic_thread, NULL, (void *) dynamic_pool_size_update, (void *) &pargs)) != 0){
		printf("Failed to create dynamic thread.\n");
		exit(1);
	}
	else{		// Thread created successfully, detach it here
		if(pthread_detach(dynamic_thread) !=0){
			printf("Failed to detach dynamic thread.\n");
			exit(1);
		}
	}

  }
  else{	// Turn on all threads in pool
	for(int i = 0; i < MAX_THREADS; i++){
		flags[i] = 1;
	}
  }

  // Wait for other threads to terminate
  while(doneFlag){
	sleep(1);
  }

  

  // Terminates Server Gracefully
  // Prints pending requests
  pthread_mutex_lock(&ringBufferLock);							// These locks are not released because
  printf("Pending Requests: %d\n", pending_requests);			// we don't want any other threads to 
  																// access their content because we are
																// exiting.
  
  // Closes the logfile
  pthread_mutex_lock(&logFileLock);
  if(fclose(logFile) == 0){		// Ensures logfile closed properly
  	printf("Closed Log File Successfully.\n");
  }
  // Removes the cache
  pthread_mutex_lock(&cacheLock);
  deleteCache();



  return 0;
}
