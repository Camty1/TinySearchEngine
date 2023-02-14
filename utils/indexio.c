#include <indexio.h>
#include <string.h>

#define HASH_SIZE 100

typedef struct {
    char* word;
    queue_t* documentQueue;
} wordQueue_t;

typedef struct {
    int documentId;
    int count;
} docWordCount_t;

static FILE* outputFile;
static FILE* inputFile;

static void printWordToFile(void* elementp);
static void printQueueToFile(void* elementp);

int indexSave(hashtable_t* index, char* indexnm) {
    outputFile = fopen(indexnm, "w");

    happly(index, printWordToFile);

    fclose(outputFile);
    return 0;
}

hashtable_t* indexLoad(char* indexnm) {
    char buffer[100];
    inputFile = fopen(indexnm, "r");

    hashtable_t* index = hopen(HASH_SIZE);

    int readState = 0;
    int lengthCounter = 0;
    char ch;
    char* end;

    char* word;
    queue_t* queue;
    wordQueue_t* wq;
    docWordCount_t* doc;
    int documentId;
    int count;

    while ((ch = fgetc(inputFile)) != EOF) {
        if (readState == 0) {
            // Still reading string
            if (ch != ' ') {
                buffer[lengthCounter] = ch;
                lengthCounter++;
            }
            // Reached end of string
            else {
                //  Terminate string
                buffer[lengthCounter] = '\0';

                // Allocate memory on heap for word and copy from buffer
                word = malloc((lengthCounter + 1) * sizeof(char));
                strcpy(buffer, word);
                
               // Create queue for wordQueue_t object
                queue = qopen();

                // Allocate word queue on heap and set values
                wq = malloc(sizeof(wordQueue_t));
                wq->word = word;
                wq->documentQueue = queue;

                // Put word queue into index
                hput(index, wq, word, strlen(word));

                // Set length back to 0 and move onto next state
                lengthCounter = 0;
                readState = 1;
            }
        }
        else if (readState == 1) {
            if (ch != ' ') {
                buffer[lengthCounter] = ch;
                lengthCounter++;
            }
            else {
                // Terminate string
                buffer[lengthCounter] = '\0';

                // Convert string to documentId
                documentId = strtol(buffer, &end, 10);

                // Reset length counter and move to next state
                lengthCounter = 0;
                readState = 2;
            }
        }
        else {
            // Has not reached end of number yet
            if (ch != ' ' && ch != '\n') {
                // Add character to buffer
                buffer[lengthCounter] = ch;
                lengthCounter++;
            }

            // Reached end of number
            else {
                // Terminate string
                buffer[lengthCounter] = '\0';

                // Convert string to int
                count = strtol(buffer, &end, 10);

                // Another document to be added, go back to state 1
                if (ch == ' ') {
                    readState = 1;
                }

                // Last document for this word, new line and new word
                else {
                    readState = 0; 
                }

                // Reset length counter
                lengthCounter = 0;

                // Allocate memory on heap for docWordCount_t object
                doc = (docWordCount_t*) malloc(sizeof(docWordCount_t));

                // Assign variables
                doc->documentId = documentId;
                doc->count = count;

                // Add to word queue
                qput(wq->documentQueue, doc);
            }
        }
    }
    return index;
}

static void printWordToFile(void* elementp) {
    wordQueue_t* wq = (wordQueue_t*) elementp;
    fprintf(outputFile, "%s", wq->word);
    qapply(wq->documentQueue, printQueueToFile);
    fprintf(outputFile, "\n");
}

static void printQueueToFile(void* elementp) {
    docWordCount_t* doc = (docWordCount_t*) elementp;
    fprintf(outputFile, " %d %d", doc->documentId, doc->count);
}

