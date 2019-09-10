#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

#define MAXLINE 256

extern int debug;

extern struct frame *coremap;

extern char *tracefile;

unsigned long *trace_history;//an array contains all traces from the trace file(.ref)
int trace_length;
int trace_idx;//also used as an time indicator (higher idx means the use/reference at more later time)
			  //because the trace file is produced per the time sequence
unsigned long *phy_mem;//an array contains vaddr for the lookup in the trace_history

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {

	/*
	Given the trace_history, OPT is implemented to, at any time point (trace_idx), find the page 
	in phy_mem that's referenced/used as late as possible in trace_history. 
	*/
	
	int i, j, frame;
	unsigned long max_delay = 0;

	for(i = 0; i < memsize; i++)
	{
		for (j = trace_idx; j < trace_length; j++)
		{
			if (phy_mem[i] == trace_history[j])
			{
				if (j >= max_delay)
				{
					max_delay = j;
					frame = i;
				}
				break;			
			}			
		}
		if (j == trace_length)//reach end of trace_history
			return i;		
	}	
	return frame;
	
	
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {

	int pge_frame = p->frame >> PAGE_SHIFT;//restore actual page frame number
	phy_mem[pge_frame] = trace_history[trace_idx];//page is accessed in the time sequence as well. 
	trace_idx++;//incremental indicates from earily to later time.
	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	
	phy_mem = malloc(memsize * sizeof(unsigned long));

	//open trace file
	FILE *trf;
	if ((trf = fopen(tracefile, "r")) == NULL)
	{
		perror("Faile to open trace file!");
		exit(1);
	}

	//count the number of traces to get trace_length
	char buf[MAXLINE];
	addr_t vaddr = 0;
	char type;
	trace_length = 0;

	while(fgets(buf, MAXLINE, trf) != NULL) 
	{
		if(buf[0] != '=') 
			trace_length++;			
		else 
			continue;
	}

	//create trace_history	
	trace_history = malloc(trace_length * sizeof(unsigned long));
	
	//read each trace into trace_history
	int i = 0;	
	while(fgets(buf, MAXLINE, trf) != NULL) 
	{
		if(buf[0] != '=')
		{ 
			sscanf(buf, "%c %lx", &type, &vaddr);
			trace_history[i] = vaddr;
			i++;			
		}
		else 
			continue;
	}

	trace_idx = 0;//means starts at eariliest time.

	//close trace file
	fclose(trf);

}

