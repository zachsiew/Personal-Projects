#include "reducer.h"

// --- Global Variables ---
finalKeyValueDS* rds; // Declares the finalKeyValueDS

// create a key value node
finalKeyValueDS *createFinalKeyValueNode(char *word, int count){
	finalKeyValueDS *newNode = (finalKeyValueDS *)malloc (sizeof(finalKeyValueDS));
	strcpy(newNode -> key, word);
	newNode -> value = count;
	newNode -> next = NULL;
	return newNode;
}

// insert or update an key value
finalKeyValueDS *insertNewKeyValue(finalKeyValueDS *root, char *word, int count){
	finalKeyValueDS *tempNode = root;
	if(root == NULL)
		return createFinalKeyValueNode(word, count);
	while(tempNode -> next != NULL){
		if(strcmp(tempNode -> key, word) == 0){
			tempNode -> value += count;
			return root;
		}
		tempNode = tempNode -> next;
	}
	if(strcmp(tempNode -> key, word) == 0){
		tempNode -> value += count;
	} else{
		tempNode -> next = createFinalKeyValueNode(word, count);
	}
	return root;
}

// free the DS after usage. Call this once you are done with the writing of DS into file
void freeFinalDS(finalKeyValueDS *root) {
	if(root == NULL) return;

	finalKeyValueDS *tempNode = root -> next;;
	while (tempNode != NULL){
		free(root);
		root = tempNode;
		tempNode = tempNode -> next;
	}
}

// reduce function
void reduce(char *key) {
	FILE *fh = fopen(key, "r");  // Open the file
	char word [100];		     // Store the respective word
	int i = 0;                   // Counter
	
	// Checks if file was opened successfully
	if (fh == NULL){
		printf("Could not open reducer file.\n");
		exit(1);
	}

	int ret_val = fscanf(fh, "%s", word);             		// Get the word and the first number from the file
	if (ret_val == EOF){									// Makes sure the word was read successfully
		printf("Errored while reading word in reducer.c");
		exit(1);
	}
	while(fscanf(fh, "%d", &i) != EOF){			// Loop to find how many 1's are in the file
		rds = insertNewKeyValue(rds, word, i);	// Inserts key value into the finalKeyValueDS
	}
	
	if(fclose(fh) == EOF){				// Makes sure file was closed properly
		printf("Failed to close reducer file properly.");
		exit(1);
	}
}

// write the contents of the final intermediate structure
// to output/ReduceOut/Reduce_reducerID.txt
void writeFinalDS(int reducerID){
	finalKeyValueDS* root = rds; 				// Get root node of data structure
	char fp[51];
	sprintf(fp, "./output/ReduceOut/Reduce_%d.txt", reducerID); // File path
	FILE* fh = fopen(fp, "w");
	// Checks if file was opened successfully
	if (fh == NULL){
		printf("Could not open reducer_ID.txt file.\n");
		exit(1);
	}

	while (root != NULL){ 								// While there are still words left
		fprintf(fh, "%s %d\n", root->key, root->value); // Write word and values to file
		root = root->next; 								// Iterate through data structure to next word
	}
	if (fclose(fh) == EOF){ 		// Close file
		printf("Failed to close output reducer file.");
		exit(1);
	}
	freeFinalDS(rds);	// Frees final data structure
}

int main(int argc, char *argv[]) {

	if(argc < 2){
		printf("Less number of arguments.\n");
		printf("./reducer reducerID");
	}

	// ###### DO NOT REMOVE ######
	// initialize
	int reducerID = strtol(argv[1], NULL, 10);

	// ###### DO NOT REMOVE ######
	// master will continuously send the word.txt files
	// alloted to the reducer
	char key[MAXKEYSZ];
	while(getInterData(key, reducerID))
		reduce(key);

	// You may write this logic. You can somehow store the
	// <key, value> count and write to Reduce_reducerID.txt file
	// So you may delete this function and add your logic
	writeFinalDS(reducerID);

	return 0;
}
