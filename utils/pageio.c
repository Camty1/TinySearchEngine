#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <webpage.h>
#include <pageio.h>

int32_t pagesave(webpage_t* pagep, int id, char* dirnm) {

    // Retrieve relevant information about given webpage
    char* url = webpage_getURL(pagep);
    int depth = webpage_getDepth(pagep);
    int html_len = webpage_getHTMLlen(pagep);
    char* html = webpage_getHTML(pagep);

    // File location
    char filepath[100];
    sprintf(filepath, "%s/%d", dirnm, id);

    // Open file (creates file if does not exist, otherwise overwrites it)
    FILE* output_file;
    output_file = fopen(filepath, "w+");

    if (output_file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    fprintf(output_file, "%s\n", url);
    fprintf(output_file, "%d\n", depth);
    fprintf(output_file, "%d\n", html_len);
    fprintf(output_file, "%s\n", html);

    int success = fclose(output_file);

    return success;
}

webpage_t* pageload(int id, char* dirnm) {
    return NULL;
}
