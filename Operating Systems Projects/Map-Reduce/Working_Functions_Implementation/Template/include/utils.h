#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/ipc.h>

#define chunkSize 1024
#define MSGSIZE 1100
#define PERM 0666		// Permissions for message queue
#define Q1 100			// Helper id for msg queue 1
#define Q2 200			// Helper id for msg queue 2

struct msgBuffer {
    long msgType;
    char msgText[chunkSize];
};

// mapper side
int validChar(char c);
char *getWord(char *chunk, int *i);
char *getChunkData(int mapperID);
void sendChunkData(char *inputFile, int nMappers);


// reducer side
int hashFunction(char* key, int reducers);
int getInterData(char *key, int reducerID);
void shuffle(int nMappers, int nReducers);

// directory
void createOutputDir();
char *createMapDir(int mapperID);
void removeOutputDir();
void bookeepingCode();

// msg queue
void closeMsgQueue(char *word, int id_queue_helper);

#endif
