#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

int sockfd;

/**********************************************
 * init
   - port is the number of the port you want the server to be
     started on
   - initializes the connection acception/handling system
   - YOU MUST CALL THIS EXACTLY ONCE (not once per thread,
     but exactly one time, in the main thread of your program)
     BEFORE USING ANY OF THE FUNCTIONS BELOW
   - if init encounters any errors, it will call exit().
************************************************/
// This code was taken from the socket template provided by Dr. Weissman
void init(int port) {
  struct sockaddr_in sock_addr;
  int enable = 1;

  //Create socket
  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
    perror("Can't create socket.");
    exit(1);
  }

  // ------  Binds socket, and makes its address reusable  -----
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons(port);
  sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  /* Note the use of INADDR_ANY to get the OS to fill in the local
  host address. You may not want to do this on a multi-home
  host. */


  /*Set the 'reuse' option so we can shut down and restart the server
    without waiting for the standard timeout. */
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1){
    perror("Could not set 'reuse' option.");
    exit(1);
  }

  //Bind the socket
  if (bind(sockfd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) == -1){
    perror("Could not bind socket.");
    exit(1);
  }

  //Setup socket to listen for incoming requests
  if (listen(sockfd, 20) == -1){ // server can accept a backlog of up to 20 connection requests
    perror("Could not setup socket to listen.");
    exit(1);
  }

}

/**********************************************
 * accept_connection - takes no parameters
   - returns a file descriptor for further request processing.
     DO NOT use the file descriptor on your own -- use
     get_request() instead.
   - if the return value is negative, the request should be ignored.
***********************************************/
int accept_connection(void) {
  int client_fd;
  struct sockaddr_in client_addr;
  unsigned int addr_size = sizeof(client_addr);

  // Client_fd is how we communicate with the corresponding client
  if ((client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size)) == -1){
    perror("Failed to accept connection.");
    return -1;
  }

  return client_fd;
}

/**********************************************
 * get_request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        from where you wish to get a request
      - filename is the location of a character buffer in which
        this function should store the requested filename. (Buffer
        should be of size 1024 bytes.)
   - returns 0 on success, nonzero on failure. You must account
     for failures because some connections might send faulty
     requests. This is a recoverable error - you must not exit
     inside the thread that called get_request. After an error, you
     must NOT use a return_request or return_error function for that
     specific 'connection'.
************************************************/
int get_request(int fd, char *filename) {
  char buff[2048];
  int readsz = 0;
  char buff1[10];
  char buff2[1024];
  char buff3[100];
  if ((readsz = read(fd, buff, 2047)) >= 0) {
    buff[readsz] = '\0';
    printf("%s\n", buff);
    if (sscanf(buff, "%s %s %s", buff1, buff2, buff3) < 2){
        printf("Could not parse into 3 strings by space. %s\n", buff);
		if(close(fd) != 0){
		  printf("Failed to close fd.\n");
		  exit(1);
		}
        return -1;
    }

    // Ensuring request type is GET
    if (strcmp("GET", buff1) != 0) { 
        printf("Request was not GET. %s\n", buff1);
		if(close(fd) != 0){
		  printf("Failed to close fd.\n");
		  exit(1);
		}
        return -1;
    }

    // Checking for .. and //
    int foundPeriod = 0;
    int foundSlash = 0;
    for(int i = 0; i < strlen(buff2); i++){
        if (buff2[i] == '.'){
            if (foundPeriod){
                printf("Found \"..\" in url. Not allowed you dirty hacker.\n");
				if(close(fd) != 0){
				  printf("Failed to close fd.\n");
				  exit(1);
				}
                return -1;
            }
            foundPeriod = 1;
        }
        else{
            foundPeriod = 0;
        }
        if (buff2[i] == '/'){
            if (foundSlash){
                printf("Found \"//\" in url. Not allowed you dirty hacker.\n");
				if(close(fd) != 0){
				  printf("Failed to close fd.\n");
				  exit(1);
				}
                return -1;
            }
            foundSlash = 1;
        }
        else{
            foundSlash = 0;
        }
    } 
  }
  else {
    perror("Server read problem.\n");
  }


  strcpy(filename, buff2);
  return 0;
}

/**********************************************
 * return_result
   - returns the contents of a file to the requesting client
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the result of a request
      - content_type is a pointer to a string that indicates the
        type of content being returned. possible types include
        "text/html", "text/plain", "image/gif", "image/jpeg" cor-
        responding to .html, .txt, .gif, .jpg files.
      - buf is a pointer to a memory location where the requested
        file has been read into memory (the heap). return_result
        will use this memory location to return the result to the
        user. (remember to use -D_REENTRANT for CFLAGS.) you may
        safely deallocate the memory after the call to
        return_result (if it will not be cached).
      - numbytes is the number of bytes the file takes up in buf
   - returns 0 on success, nonzero on failure.
************************************************/
int return_result(int fd, char *content_type, char *buf, int numbytes) {
  char hline1[100];
  char hline2[100];
  char hline3[100];
  char hline4[100];
  char msg[10234]; 

  strcpy(hline1, "HTTP/1.1 200 OK\n");
  sprintf(hline2, "Content-Type: %s\n", content_type);
  sprintf(hline3, "Content-Length: %d\n", numbytes);
  strcpy(hline4, "Connection: Close\n");
  sprintf(msg, "%s%s%s%s\n", hline1, hline2, hline3, hline4); 
  // Printing Header and message.
  printf("%s\n", msg);
  for(int i = 0; i < numbytes; i++){
    printf("%c", buf[i]);
  }

  if (write(fd, msg, strlen(msg)) != strlen(msg)){
    printf("Failed to write header.\n");
    return -1;
  }
  if (write(fd, buf, numbytes) != numbytes){
    printf("Failed to write file to client. \n");
    return -1;
  } 

  // Freeing file from memory SERVER.C IS ALREADY FREEING
  //free(buf);

  // Closing child socket
  if (close(fd) != 0){
    printf("Failed to close child socket.\n");
    return -1;
  }

  return 0;

}

/**********************************************
 * return_error
   - returns an error message in response to a bad request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the error
      - buf is a pointer to the location of the error text
   - returns 0 on success, nonzero on failure.
************************************************/
int return_error(int fd, char *buf) {
  char hline1[100];
  char hline2[100];
  char hline3[100];
  char hline4[100];
  char msg[10234]; 

  strcpy(hline1, "HTTP/1.1 404 Not Found\n");
  strcpy(hline2, "Content-Type: text/html\n");
  sprintf(hline3, "Content-Length: %ld\n", sizeof(buf));
  strcpy(hline4, "Connection: Close\n");
  sprintf(msg, "%s%s%s%s\n", hline1, hline2, hline3, hline4); 
  printf("%s", msg); 
  if (write(fd, msg, strlen(msg)) != 0){
    printf("Failed to write header.\n");
    return -1;
  }
  if (write(fd, buf, sizeof(buf)) < 0 ){
    printf("Failed to write file to client. \n");
    return -1;
  } 

  // Closing child socket
  if (close(fd) != 0){
    printf("Failed to close child socket.\n");
    return -1;
  }

  return 0;
}




