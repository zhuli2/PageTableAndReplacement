#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int first_in_page;//used to keep track of the oldest page in the physical memory (i.e., coremap)

/* Page to evict is chosen using the fifo algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int fifo_evict() {	
	/*
	Given the coremap where the (physical) pages are appended one by one,
	then the first-in pages has indices from 0 up to memsize. When the 
	coremap (representing physical memory) is full, the first-in pages are 
	replaced before the later-in pages (FIFO). Note, once one later-in page
	replaces the oldest first-in page, then it has index smaller than all 
	remaining first-in pages. So must keep track of the oldest first-in page
	using %memsize because its index ranges from zero up to memsize.   
	*/
	
	int idx       = first_in_page;
	first_in_page = (first_in_page + 1) % memsize;
	return idx;
}

/* This function is called on each access to a page to update any information
 * needed by the fifo algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void fifo_ref(pgtbl_entry_t *p) {
	/*
	For FIFO algorithm, nothing to do for the page reference because the page
	eviction doesn't depend on the reference.
	*/
	return;
}

/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void fifo_init() {

	first_in_page = 0;
}
