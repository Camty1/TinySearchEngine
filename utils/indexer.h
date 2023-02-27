#pragma once

/* indexer.h --- 
 * 
 * 
 * Author: Hannah M. Brookes
 * Created: Sun Feb 26 19:06:41 2023 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>                                                                      
#include <stdlib.h>                                                                     
#include <stdint.h>                                                                     
#include <string.h>                                                                     
#include <ctype.h>                                                                      
#include <queue.h>                                                                      
#include <hash.h>                                                                       
#include <webpage.h>                                                                    
#include <pageio.h>                                                                     
#include <indexio.h>

typedef struct {                                                                        
    char* word;                                                                        
    queue_t* documentQueue;                                                             
} wordQueue_t;                                                                          
                                                                                        
typedef struct {                                                                        
    int documentId;                                                                     
    int count;                                                                          
} docWordCount_t;

typedef struct {                                                                        
    char* word;                                                                         
    int count;                                                                          
} wordCount_t;

void closeIndex(hashtable_t* index);
hashtable_t* index_all_pages(char* dirnm, char* indexnm);
void removeDocCount(void* elementp);
