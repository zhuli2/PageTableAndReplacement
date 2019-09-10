#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int *ref_clock;//a circular array whose indices are the pages' frames to record
                //if one page is referenced (1) or not (0). 
				//So, ref_clock[page's actual frame] = 0/1 := ref(erence) bit of the page

int clockhand; //used as iterator to loop over ref-clock until one page old enough is selected for swap-out.   

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	/*
	Similar to the Second-chance algorithm (derived from FIFO), the clock algorithm doesn't replace
	an old page immediately but give it a second chance: if the old page is referenced recently
	(ref_clock[page's actual frame] = 1), then reset its ref bit to 0 (so giving second chance); Otherwise
	(not referenced recently and then ref bit = 0), replace the old page immediately by returing its actual
	fram number (index of ref_clock). The clock circle (a loop) always starts from clockhand that's located	
	on the oldest page (idx) that's replaced most recently. The clock is circulating continuously until a 
	replacement occurs.  
	*/
		
	while (clockhand < memsize)
	{
		if (ref_clock[clockhand] == 1)//if referenced (1), then reset it to be un-referenced (0) -- second chance.
			ref_clock[clockhand] = 0;
		else
			break;
						
		//update clockhand using %memsize to make ref_clock a circular list.
		clockhand = (clockhand + 1) % memsize;
	}
	
	return clockhand;	
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	/*
	If a page (p) is referenced, then update ref_clock[p's actual frame] = 1 (default is 0).	
	*/

	//restore actual the page's frame number (to be used as an index)
	int idx        = p->frame >> PAGE_SHIFT;
	ref_clock[idx] = 1;	
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {

	clockhand = 0;//initialize to locate on the oldest page
	ref_clock = malloc(sizeof(int) * memsize);
	
	//initialize to be Un-referenced (0)
	int i;
	for (i = 0; i < memsize; i++)
	{
		ref_clock[i] = 0;
	}

	
}
