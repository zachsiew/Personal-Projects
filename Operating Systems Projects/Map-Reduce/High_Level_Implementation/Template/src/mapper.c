#include "mapper.h"


//--- Global Variables ---
intermediateDS* ds = NULL; // Root node of intermediate data structure


// combined value list corresponding to a word <1,1,1,1....>
valueList *createNewValueListNode(char *value){
	valueList *newNode = (valueList *)malloc (sizeof(valueList));
	strcpy(newNode -> value, value);
	newNode -> next = NULL;
	return newNode;
}

// insert new count to value list
valueList *insertNewValueToList(valueList *root, char *count){
	valueList *tempNode = root;
	if(root == NULL)
		return createNewValueListNode(count);
	while(tempNode -> next != NULL)
		tempNode = tempNode -> next;
	tempNode -> next = createNewValueListNode(count);
	return root;
}

// free value list
void freeValueList(valueList *root) {
	if(root == NULL) return;

	valueList *tempNode = root -> next;;
	while (tempNode != NULL){
		free(root);
		root = tempNode;
		tempNode = tempNode -> next;
	}
}

// create <word, value list>
intermediateDS *createNewInterDSNode(char *word, char *count){
	intermediateDS *newNode = (intermediateDS *)malloc (sizeof(intermediateDS));
	strcpy(newNode -> key, word);
	newNode -> value = NULL;
	newNode -> value = insertNewValueToList(newNode -> value, count);
	newNode -> next = NULL;
	return newNode;
}

// insert or update a <word, value> to intermediate DS
intermediateDS *insertPairToInterDS(intermediateDS *root, char *word, char *count){
	intermediateDS *tempNode = root;
	if(root == NULL)
		return createNewInterDSNode(word, count);
	while(tempNode -> next != NULL) {
		if(strcmp(tempNode -> key, word) == 0){
			tempNode -> value = insertNewValueToList(tempNode -> value, count);
			return root;
		}
		tempNode = tempNode -> next;

	}
	if(strcmp(tempNode -> key, word) == 0){
		tempNode -> value = insertNewValueToList(tempNode -> value, count);
	} else {
		tempNode -> next = createNewInterDSNode(word, count);
	}
	return root;
}

// free the DS after usage. Call this once you are done with the writing of DS into file
void freeInterDS(intermediateDS *root) {
	if(root == NULL) return;

	intermediateDS *tempNode = root -> next;;
	while (tempNode != NULL){
		freeValueList(root -> value);
		free(root);
		root = tempNode;
		tempNode = tempNode -> next;
	}
}

// emit the <key, value> into intermediate DS
void emit(char *key, char *value) {
		//creates node if it does not already exist, otherwise appends
		//a '1' to the end of the linked list corresponding to the key
	 ds = insertPairToInterDS(ds, key, value); // Emits data into intermediate DS
}

// map function
void map(char *chunkData){
	int i = 0; 
	char* buffer;
	while ((buffer = getWord(chunkData, &i)) != NULL){ // Reads all words from a file
		emit(buffer, "1");							   // Emits words and their count into the intermediate DS
	}
}

// write intermediate data to separate word.txt files
// Each file will have only one line : word 1 1 1 1 1 ...
void writeIntermediateDS() {
	intermediateDS* root = ds;			// Creates pointer to root node
	while(root != NULL){				// Iterates through nodes of intermediate DS
		char* word = root->key; 		// Get word/key from current node
		char fp[51];					// Assumes the filepath wont be greater than 50 characters
		sprintf(fp, "./output/MapOut/Map_%d/%s.txt", mapperID, word);	// File Path to store word in
		FILE* fh = fopen(fp,"w");
		// Checks to make sure file handle opened successfully
		if (fh == NULL){
			printf("Could not open file path %s.\n", fp);
			exit(1);
		}

		int ret_val = fprintf(fh, "%s", word); 		// Writes current word to file
		// Checks to see if fprintf was successful
		if (ret_val < 0 || ret_val != strlen(word)){
			printf("Errored while writing in mapper.c");
			exit(1);
		}
		valueList* val = root->value;		// Creates pointer to value to iterate thogh counts of words
		while(val != NULL){ 				// while there are still '1's
			ret_val = fprintf(fh, " 1"); 	// Print a 1 for each 1 in data structure corresponding to current word
			if (ret_val != 2){				// Makes sure the write was successful
				printf("Errored while writing in mapper.c");
				exit(1);
			}
			val = val->next; 			// Iterates through linked list of '1's
		}
		if(fclose(fh) == EOF){			// Close current word file and check for success
			printf("Could not close file.");
			exit(1);
		}
		root = root->next;  			// Updater, move to next word in data structure
	}
	freeInterDS(ds); 					// Free the intermediate data structure once done writing to files
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		printf("Less number of arguments.\n");
		printf("./mapper mapperID\n");
		exit(0);
	}
	// ###### DO NOT REMOVE ######
	mapperID = strtol(argv[1], NULL, 10);

	// ###### DO NOT REMOVE ######
	// create folder specifically for this mapper in output/MapOut
	// mapOutDir has the path to the folder where the outputs of
	// this mapper should be stored
	mapOutDir = createMapDir(mapperID);

	// ###### DO NOT REMOVE ######
	while(1) {
		// create an array of chunkSize=1024B and intialize all
		// elements with '\0'
		char chunkData[chunkSize + 1]; // +1 for '\0'
		memset(chunkData, '\0', chunkSize + 1);

		char *retChunk = getChunkData(mapperID);
		if(retChunk == NULL) {
			break;
		}

		strcpy(chunkData, retChunk);
		free(retChunk);

		map(chunkData);
	}

	// ###### DO NOT REMOVE ######
	writeIntermediateDS();

	return 0;
}
