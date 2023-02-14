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
    fclose(inputFile);

    hashtable_t* index = hopen(HASH_SIZE);

    int readState = 0;
    int lengthCounter = 0;
    char ch;

    char* word;
    queue_t* queue;
    wordQueue_t* wq;
    docWordCount_t* doc;

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
                // TODO: Convert string to int, change state, reset length counter
            }
        }
        else {
            // TODO: Traverse string, check for ' ' or '\n', convert string to int, make docWordCount_t object, push to queue, chage state
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

