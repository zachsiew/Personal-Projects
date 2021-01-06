#include "utils.h"

char *getChunkData(int mapperID) {
	key_t key = ftok("./src/mapper.c", Q1);		// Gets key for message queue (talks to mapper)
	int msgid = msgget(key, PERM | IPC_CREAT);  // Opens msg_queue
	struct msgBuffer msg;						// creates struct to store message in
	char *data = malloc(sizeof(msg.msgText)); 	// Allocates bytes for the message
	memset(msg.msgText, '\0', chunkSize);

	// Ensures key was generated properly
	if (key == -1){
		printf("Failed to generate message queue key in getChunkData.\n");
		exit(0);
	}
	// Checks to see if opened msg queue
	if (msgid == -1){
		printf("Failed to open message queue: %d\n", key);
		exit(0);
	}
	// Ensures malloc passed
	if (data == NULL){
		printf("Failed to malloc bytes for message.\n");
		exit(0);
	}
	// Reads message from queue
	if (msgrcv(msgid, (void *) &msg, sizeof(msg.msgText), mapperID, IPC_NOWAIT) == -1){
		printf("Failed to read message from message queue.\n");
		exit(0);
	}
	// Checks for the END message
	if (strcmp(msg.msgText, "END") == 0){
		return NULL;
	}
	// Returns data
	else{
		strcpy(data, msg.msgText);
		return data;
	}
}

// sends chunks of size 1024 to the mappers in round robin fashion
void sendChunkData(char *inputFile, int nMappers) {
	key_t key = ftok("./src/mapper.c", Q1);     // Gets key for message queue
	int msgid = msgget(key, PERM | IPC_CREAT);  // Creates msg_queue
	struct msgBuffer msg;						// Struct for the message
	msg.msgType = 1;							// Populate msg struct
	char word[100];								// Buffer to store words in. (Assume word less than 99 characters.)
	int pos = 0; 								// Keeps track of data inside msg.msgText
	int wi = 0;									// Keeps index within word bufer
	long mapperid = 0;							// Mapper id to send data to
	char c[1] = "\0";							// Buffer for reading one byte at a time
	int fd = open(inputFile, O_RDONLY);			// Opens inputFile for reading
	memset(word, '\0', 100);					// Primes word to all '\0' values
	memset(msg.msgText, '\0', chunkSize);		// Primes msg.msgText to all '\0' values

	// ERROR Handling
	
	// Ensures key was generated properly
	if (key == -1){
		printf("Failed to generate message queue key in getChunkData.\n");
		exit(0);
	}
	// Checks to see if opened msg queue
	if (msgid == -1){
		printf("Could not open message queue.\n");
		exit(0);
	}
	// Checks for error with opening a file
	if (fd == -1){
		printf("Could not open file: %s\n", inputFile);
		exit(0);
	}

	// Reads through file one character at a time
	while(read(fd, c, 1) > 0){
		if(c[0] == ' ' || c[0] == '\n' || c[0] == '\t' || c[0] == '\0'){		// Checks if current character distinguishes a word
			word[wi] = c[0];													// Adds character to word and updates word index
			wi++;
			if(strlen(msg.msgText) + strlen(word) > chunkSize){					// Checks if word can fit in current buffer
				// Word CAN'T fit inside buffer, so flush 
				// buffer and write word to buffer
				msg.msgType = (mapperid % nMappers) + 1;						// Updates which mapper to send data too
				if (msgsnd(msgid, (void *) &msg, sizeof(msg.msgText),0) == -1){	// Sends message to mapper
					printf("Failed to send message to mapper %ld.", msg.msgType);
				}
				mapperid++;														
				pos = 0;									// Resets buffer index and resets memory
				memset(msg.msgText, '\0', chunkSize);

				strcpy(msg.msgText, word);	// Copies word into fresh buffer
				pos += strlen(word);		// Updates buffer index
				memset(word, '\0', 100);	// Resets word memory and index
				wi = 0;
			}
			// Word CAN fit inside buffer, so add it to buffer
			else{
					strcpy(&msg.msgText[pos], word);		// Copy word into buffer
					pos += strlen(word);					// Update buffer index
					wi = 0;									// Reset word index and reset word data back to '\0' characters
					memset(word, '\0', 100);
			}
		}
		// Not a word, so just add character to word buffer
		else{	
			word[wi] = c[0];		// Adds character to word buffer and updates 
			wi++;					// word index
		}
	}

	close(fd);			// Closes the file once done with reading


	// Done iterating though file, but data still left in buffer	
	
	msg.msgType = (mapperid % nMappers) + 1; // Update which mapper to send data to 

	// Copies last word into buffer
	strcpy(&msg.msgText[pos], word);
	pos += strlen(word);		// Resets word buffer and updates buffer position
	memset(word,'\0', 100);		
	wi = 0;

	// Sends rest of buffer into msg queue
	if(msgsnd(msgid, (void *) &msg, sizeof(msg.msgText), 0) == -1){
		printf("%ld\n", msg.msgType);
		printf("Errored when sending message.\n");
		exit(0);
	}


	// Sending the END messages to mappers
	memset(msg.msgText, '\0', chunkSize);		 // Resets memory to all null characters
	strcpy(msg.msgText, "END");					 // The end message
	for(int i = 1; i <= nMappers; i++){
		msg.msgType = i;		// For specific mapper i
		if(msgsnd(msgid, (void *) &msg, sizeof(msg.msgText), 0) == -1){	// Sends END message
			printf("Errored when sending message.\n");
			exit(0);
		}
	}
	return;
}

// hash function to divide the list of word.txt files across reducers
//http://www.cse.yorku.ca/~oz/hash.html
int hashFunction(char* key, int reducers){
	unsigned long hash = 0;
    int c;

    while ((c = *key++)!='\0')
        hash = c + (hash << 6) + (hash << 16) - hash;

    return (hash % reducers);
}

int getInterData(char *key, int reducerID) {
	key_t q_key = ftok("./src/reducer.c", Q2);			// Gets key for message queue
	int msgid = msgget(q_key, PERM | IPC_CREAT);		// Opens message queue
	struct msgBuffer msg;								// Store message
	
	// Ensures key was generated properly
	if (q_key == -1){
		printf("Failed to generate message queue key in getInterData.\n");
		exit(0);
	}

	// Checks to see if opened msg queue
	if (msgid == -1){
		printf("Failed to open message queue: %d\n", q_key);
		exit(0);
	}

	// Reads message into msg.msgText with filter reducerID
	if (msgrcv(msgid, (void *) &msg, sizeof(msg.msgText), reducerID, 0) == -1){
		printf("Failed to read message from message queue.\n");
		exit(0);
	}

	// Checks to see if END message recieved
	if (strcmp(msg.msgText, "END") == 0){
		return 0;
	}
	else{
		strcpy(key, msg.msgText);
		return 1;
	}

}

void shuffle(int nMappers, int nReducers) {
	key_t key = ftok("./src/reducer.c", Q2);	// Gets key for message queue
	int msgid = msgget(key, PERM | IPC_CREAT);  // Opens msg_queue
	DIR *dp;									// Used for navigating though the directiores
	struct dirent *entry;
	int reducerID;								// Which reducer to send output to
	struct msgBuffer msg;						// Used to send message to reducer

	// Ensures key was generated properly
	if (key == -1){
		printf("Failed to generate message queue key in shuffle.\n");
		exit(0);
	}
	// Checks to see if opened msg queue
	if (msgid == -1){
		printf("Failed to open message queue.\n");
		exit(0);
	}

	// Iterating through mappers
	for(int i = 1; i <= nMappers; i++){
		// Iterate through directory entries
		char *map_dir = createMapDir(i);			// Creates MapperID directory path
		dp = opendir(map_dir);						// Opens directly to read the different words
		if(dp == NULL){								// Error checks directory opening
			printf("Failed to open directory: %s", map_dir);
			exit(0);
		}
		// Read throgh all directory entries
		while((entry = readdir(dp)) != NULL){
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){		// Ignore directories '.' and '..'
				reducerID = hashFunction(entry->d_name, nReducers) + 1;						// Send data to specific reducer
				msg.msgType = reducerID;
				sprintf(msg.msgText, "%s/%s", map_dir, entry->d_name);						// Creates word filepath.
				if(msgsnd(msgid, (void *) &msg, sizeof(msg.msgText), 0) == -1){				// Send message to reducer
					printf("Errored when sending message.\n");
					fprintf(stderr, "%s\n", strerror (errno));
					exit(0);
				}

			}
		}
		closedir(dp);								// Closes directory pointer

	}


	// Sending the END messages to reducers
	memset(msg.msgText, '\0', chunkSize);
	strcpy(msg.msgText, "END");											// The end message
	for(int i = 1; i <= nReducers; i++){
		msg.msgType = i;
		if(msgsnd(msgid, (void *) &msg, sizeof(msg.msgText), 0) == -1){	// Sends message
			printf("Errored when sending message.\n");
			exit(0);
		}
	}
	return;
}

// check if the character is valid for a word
int validChar(char c){
	return (tolower(c) >= 'a' && tolower(c) <='z') ||
					(c >= '0' && c <= '9');
}

char *getWord(char *chunk, int *i){
	char *buffer = (char *)malloc(sizeof(char) * chunkSize);
	memset(buffer, '\0', chunkSize);
	int j = 0;
	while((*i) < strlen(chunk)) {
		// read a single word at a time from chunk
		if (chunk[(*i)] == '\n' || chunk[(*i)] == ' ' || !validChar(chunk[(*i)]) || chunk[(*i)] == 0x0) {
			buffer[j] = '\0';
			if(strlen(buffer) > 0){
				(*i)++;
				return buffer;
			}
			j = 0;
			(*i)++;
			continue;
		}
		buffer[j] = chunk[(*i)];
		j++;
		(*i)++;
	}
	if(strlen(buffer) > 0)
		return buffer;
	return NULL;
}

void createOutputDir(){
	mkdir("output", ACCESSPERMS);
	mkdir("output/MapOut", ACCESSPERMS);
	mkdir("output/ReduceOut", ACCESSPERMS);
}

char *createMapDir(int mapperID){
	char *dirName = (char *) malloc(sizeof(char) * 100);
	memset(dirName, '\0', 100);
	sprintf(dirName, "output/MapOut/Map_%d", mapperID);
	mkdir(dirName, ACCESSPERMS);
	return dirName;
}

void removeOutputDir(){
	pid_t pid = fork();
	if(pid == 0){
		char *argv[] = {"rm", "-rf", "output", NULL};
		if (execvp(*argv, argv) < 0) {
			printf("ERROR: exec failed\n");
			exit(1);
		}
		exit(0);
	} else{
		wait(NULL);
	}
}

void bookeepingCode(){
	removeOutputDir();
	sleep(1);
	createOutputDir();
}

// Hellper function that closes the msaage queues. (Called from mapreduce)
void closeMsgQueue(char *word, int id_queue_helper){
	int key = ftok(word, id_queue_helper);
	int msgid = msgget(key, PERM | IPC_CREAT);
	
	// Ensures key was generated properly
	if (key == -1){
		printf("Failed to generate message queue key in closeMsgQueue.\n");
		exit(0);
	}
	// Checks to see if opened msg queue
	if (msgid == -1){
		printf("Failed to open message queue.\n");
		exit(0);
	}

	// Close message queue
	if(msgctl(msgid, IPC_RMID, NULL) == -1){		// Closes Message queue
		printf("Failed to close message queue: %d\n", id_queue_helper);
		exit(0);
	}
	return;
}
