#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <webpage.h>
#include <pageio.h>

#define MAX_URL_SIZE 100

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
    
    // Get filepath
    char filepath[100];
    sprintf(filepath, "%s/%d", dirnm, id);
    
    // Open file for reading
    FILE* read_file; 
    read_file = fopen(filepath, "r");

    if (read_file == NULL) {
        printf("Error opening file\n");
        return NULL;
    }
    
    // Initialize stack variables for webpage creation
    char url[MAX_URL_SIZE];
    int depth;
    int html_len;
   
    // Get variables from file 
    int scan_result = fscanf(read_file, "%s %d %d ", url, &depth, &html_len); 
    
    if (scan_result != 3) {
        printf("Error reading from file first three items\n");
        return NULL;
    }
    
    // Allocate memory for html
    char* html = (char*) malloc(html_len * sizeof(char));
    
    // Create format string for fscanf
    char formatString[30];
    sprintf(formatString, "%c%d%c", '%', html_len, 'c');
		// printf("%s\n",formatString); 
    // Read from file
    scan_result = fscanf(read_file, formatString, html);

		html[html_len] = '\0';

    if (scan_result == 0) {
        printf("Error reading html\n");
        return NULL;
    }

    // Create webpage
    webpage_t* page = webpage_new(url, depth, html);

    if (page == NULL) {
        printf("Error creating webpage\n");
        return NULL;
    }
		fclose(read_file);
    return page;
}
