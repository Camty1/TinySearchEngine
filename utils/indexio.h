#pragma once
/* 
 * indexio.h -- Allows for reading and writing of hash tables for
 * Tiny Search Engine project
 *
 * Author: Cameron Wolfe
 * Date: 2/15/2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <strings.h>
#include <queue.h>
#include <hash.h>

// Take an index hashtable and save it to the file indexnm
int indexSave(hashtable_t* index, char* indexnm);

// Load an index from indexnm
hashtable_t* indexLoad(char* indexnm);
