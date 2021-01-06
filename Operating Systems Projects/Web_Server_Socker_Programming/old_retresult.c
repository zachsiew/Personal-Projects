int return_result(int fd, char *content_type, char *buf, int numbytes) {
  char errorMsg[1024];
  char contentTypeMsg[1024];
  char contentLengthMsg[1024];
  char lenBuf[1024];
  char errorBuf[5*1024];
  char *connClose = "Connection: Close";
  char tempBuf[1024];
  int  error = 0;

  // Writes data to client
  if (write(fd, buf, numbytes) != numbytes){
    printf("Failed to write bytes to fd. %d\n", fd);
    if(numbytes < 0){
      strcpy(lenBuf, "Content-Length: Invalid number of bytes.\n");
      strcpy(tempBuf, "Invalid content length.\n");
    }
    else {
      strcpy(tempBuf, "Requested file not found.\n");
      sprintf(lenBuf, "Content-Length: %d\n", numbytes);
    }
    sprintf(errorMsg, "HTTP/1.1 404 Not Found\n");
    error = 1;
  }

  if(strcmp(content_type, "text/html") == 0){
    strcpy(contentTypeMsg, "Content-Type: text/html\n");
  }
  else if(strcmp(content_type, "text/plain") == 0){
    strcpy(contentTypeMsg, "Content-Type: text/plain\n");
  }
  else if(strcmp(content_type, "image/gif") == 0){
    strcpy(contentTypeMsg, "Content-Type: image/gif\n");
  }
  else if(strcmp(content_type, "image/jpeg") == 0){
    strcpy(contentTypeMsg, "Content-Type: image/jpeg\n");
  }
  else {
    strcpy(contentTypeMsg, "Content-Type: Invalid\n");
    strcpy(tempBuf, "Invalid content type.\n");
    error = 1;
  }

  if(error){
    sprintf(errorBuf, "%s%s%s%s\n\n%s", errorMsg, contentTypeMsg, lenBuf, connClose, tempBuf);
    printf("%s\n", errorBuf);
    return_error(fd, errorBuf);
    return 1;
  }

  sprintf(lenBuf, "Content-Length: %d\n", numbytes);
  sprintf(contentLengthMsg, "Content-Length: %d\n", numbytes);

  printf("HTTP/1.1 200 OK\n");
  printf("%s", contentTypeMsg);
  printf("%s", contentLengthMsg);
  printf("Connection: close\n\n");

  printf(" DATA HERE \n");


  write(fd, "HTTP/1.1 200 OK\n", 16);
  write(fd, contentTypeMsg, sizeof(contentTypeMsg));
  write(fd, contentLengthMsg, sizeof(contentLengthMsg));
  write(fd, "Connection: Close\n", 18);
  write(fd, "\n", 1);
  write(fd, buf, sizeof((*buf)));

  close(fd);

  return 0;

}

