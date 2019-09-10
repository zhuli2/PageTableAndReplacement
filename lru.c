#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

typedef struct linked_list_node
{
	int frame;
	struct linked_list_node *next;	
} node;

node *front_node;
node *end_node;
int  *ref_record;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {

	/* 
	Evicit the page that's front node in the linked list because it's least
	recently used/referenced.  	
	*/
	
	assert(front_node != NULL); //ensure not empty linked list
    
    int victim_frame = front_node->frame; 
    ref_record[victim_frame] = 0;//unreference evicited page 
    
    if (front_node == end_node) {//sinle element list
        end_node = NULL;
    }    
    
    node* new_front_node = front_node->next;
    free(front_node);
    front_node = new_front_node;
    
    return victim_frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {

	/*
	If a page is referenced, place it to the end of linked list (nodes). So, the front
	of the list is the least used/referenced page. Note, if the page is not used for
	the first time, then it's already in the linked list. 	   
	*/

	int pge_frame = p->frame >> PAGE_SHIFT;//restore actual page from number. 
	
	//create page node regardless if referenced before or not
	node* new_node  = (node*)malloc(sizeof(node));
	new_node->frame = pge_frame;
	new_node->next  = NULL;	
	
	if (ref_record[pge_frame] == 0)//1st reference (new node)
	{
		ref_record[pge_frame] = 1;
				
		if (end_node == NULL)//empty linked list
		{
			end_node   = new_node;
			front_node = end_node;
		} else               //non-empty linked list
		{
			end_node->next = new_node;
			end_node       = new_node;
		}		
	} else //referenced before (existing node)--> non-empty linked list
	{
		end_node->next = new_node;
		end_node       = new_node;
		
		//remove duplicate page node
		node* curr_node      = front_node;
		node* previous_node  = NULL;
		
		while (curr_node->frame != pge_frame)
		{
			previous_node = curr_node;
			curr_node     = curr_node->next; 
		}
		
		if (previous_node == NULL)//duplicate is front_node
		{
			front_node = curr_node->next;
			free(curr_node);
			
			if (front_node == NULL)//empty linked list now
				end_node = NULL;		
		} else//duplicate is in the middle
		{
			previous_node->next = curr_node->next;
			free(curr_node);
		}
	}
			
	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {

	front_node = NULL;
	end_node   = NULL;	
	ref_record = malloc(sizeof(int) * memsize);	
	
	int i;
	for(i = 0; i < memsize; i++)
	{
		ref_record[i] = 0;
	}
	
}
