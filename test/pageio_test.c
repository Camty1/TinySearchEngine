/* pageio_test.c --- 
 * 
 * 
 * Author: Raif T. Olson
 * Created: Fri Feb 10 10:48:17 2023 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */


#include <stdio.h>                                                    
#include <stdlib.h>                                                   
#include <stdint.h>                                                 
#include <string.h>                                                
#include <webpage.h>
#include <pageio.h>

int main(void) {
	webpage_t *page_1;
	webpage_t *page_2;
	page_1 = pageload(1, "../pages");
	pagesave(page_1, 100, "../pages");
	page_2 = pageload(100, "../pages");
	return 0;
}
