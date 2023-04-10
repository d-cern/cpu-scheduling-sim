#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"


// PCB_st
// Linkedlist_st
/*
 * Allocates memory for a pcb node and initializes
 * its members. Returns the node.
 */
PCB_st *NewPCBnode(int id, int prio, int burst) {
	int i;
	PCB_st *node = (PCB_st *)malloc( sizeof(PCB_st) );

	if(!node) {
		fprintf(stderr, "\nERROR: malloc failed to allocate memory for PCB_st\n");
		return NULL;
	}

	node->procId = id;
	node->procPrio = prio;
	node->cpuBurst = burst;
	node->queueEnterClock = 0;
	node->waitingTime = 0;

	// set registers
	for(i = 0; i < 8; i++) 
		node->myReg[i] = id;
	
	return node;
}

/*
 * Allocates memory for a linked list and returns
 * the list.
 */
Linkedlist_st *NewLinkedList() {
	Linkedlist_st *list = (Linkedlist_st *)malloc( sizeof(Linkedlist_st) );

	if(list == NULL) {
		fprintf(stderr, "\nERROR: malloc failed to allocate memory for Linkedlist_st\n");
		return NULL;
	}

	list->head = NULL;
	list->tail = NULL;

	return list;
}

/*
 * Frees memory assigned to passed list.
 */
void FreeLinkedList(Linkedlist_st *list) {
	PCB_st *curNode = list->head;

	while(curNode != NULL) {
		list->head = curNode->next;
		free(curNode);
		curNode = list->head;
	}
	//free(curNode);
	free(list);
}

/*
 * Adds node to the end of list. If list is empty,
 * adds to beginning.
 */
void Enlist(Linkedlist_st *list, PCB_st *node) {
	node->next = NULL;

	if(list->head == NULL) {
		// debugging
		if(list->tail != NULL) {
			fprintf(stderr, "\nERROR: Enlist(); head is NULL while tail is not NULL (Delist() problem)\n");
			return;
		}

		list->head = node;
		list->tail = node;
	} else {
		list->tail->next = node;
		list->tail = node;
	}
}

/*
 * Removes head node from list and returns a pointer
 * to it.
 */
PCB_st *Delist(Linkedlist_st *list) {
	// check if list is empty
	if( LinkedListIsEmpty(list) ) {
		fprintf(stderr, "\nERROR: list is empty when calling Delist()\n");
		return NULL;
	}

	PCB_st *node = list->head;

	if(list->head == list->tail) {
		list->head = NULL;
		list->tail = NULL;
	} else {
		list->head = list->head->next;
	}

	return node;
}

/*
 * Returns 1 if list is empty, 0 if not empty.
 */
int LinkedListIsEmpty(Linkedlist_st *list) {
	if(list->head == NULL) {
		// debugging
		if(list->tail != NULL) {
			fprintf(stderr, "\nERROR: LinkedListIsEmpty(); head is NULL while tail is not NULL (Delist() problem)\n");
			return 0;
		}
		
		return 1;
	}
	else return 0;
}

/*
 * Returns the number of nodes in list.
 * TODO: weird bug causes to loop infinitely and i have no idea what is causing it
 * FIXED: problem was actually with Enlist(); was not setting node->next to null so
 *			 when i re-enlisted nodes into the list it created a circular linked list
 *			 i.e., infinite loop.
 */
int LinkedListLength(Linkedlist_st *list) {
	int length = 0;
	PCB_st *curNode;
	if(LinkedListIsEmpty(list))
		return length; // 0

	curNode = list->head;
	
	while(curNode != NULL) {
		//printf("length %d\n", length);
		curNode = curNode->next;
		length++;
	}

	return length;
}

/*
 * Returns the node at index, where 
 * list->head == index(0). Returns NULL if
 * index > LinkedListLength(list) || index < 0.
 */
PCB_st *GetLinkedListNode(Linkedlist_st *list, int index) {
	int i;
	PCB_st *curNode = list->head;

	// index = 0; return head
	if(index == 0)
		return curNode;

	for(i = 0; i < index; i++) {
		if(curNode == NULL) {
			fprintf(stderr, "\nERROR: GetLinkedListNode(); index is out of range\n");
			return NULL;
		}

		curNode = curNode->next;
	}

	return curNode;
}

PCB_st *PopMinCPUburst(Linkedlist_st *list) {
	PCB_st *prevpcb = NULL;
	PCB_st *curpcb = list->head;

	PCB_st *pcbMin_prev = NULL; // node before min node
	PCB_st *pcbMin = list->head;


	if(LinkedListIsEmpty(list))
		return NULL;
	
	// if one node
	if(list->head->next == NULL) {
		pcbMin = list->head;
		list->head = NULL;
		list->tail = NULL;
		
		return pcbMin;
	}

	while(curpcb != NULL) {
		if(curpcb->cpuBurst < pcbMin->cpuBurst) {
			pcbMin_prev = prevpcb;
			pcbMin = curpcb;
		}

		prevpcb = curpcb;
		curpcb = curpcb->next;
	}

	if(pcbMin == list->head) { 			// if min was head of list
		list->head = pcbMin->next;
	} else if(pcbMin == list->tail) { 	// if min was tail of list
		pcbMin_prev->next = NULL;
		list->tail = pcbMin_prev;
	} else {
		pcbMin_prev->next = pcbMin->next;
	}

	return pcbMin;
}

PCB_st *PopHighestPrio(Linkedlist_st *list) {
	PCB_st *prevpcb = NULL;
	PCB_st *curpcb = list->head;

	PCB_st *pcbMax_prev = NULL; // node before max node
	PCB_st *pcbMax = list->head;

	if(LinkedListIsEmpty(list))
		return NULL;

	// if one node
	if(list->head->next == NULL) {
		pcbMax = list->head;
		list->head = NULL;
		list->tail = NULL;

		return pcbMax;
	}

	while(curpcb != NULL) {
		if(curpcb->procPrio > pcbMax->procPrio) {
			pcbMax_prev = prevpcb;
			pcbMax = curpcb;
		}

		prevpcb = curpcb;
		curpcb = curpcb->next;
	}

	if(pcbMax == list->head) {				// if max was head of list
		list->head = pcbMax->next;
	} else if(pcbMax == list->tail) {	// if max was tail of list
		pcbMax_prev->next = NULL;
		list->tail = pcbMax_prev;
	} else {
		pcbMax_prev->next = pcbMax->next;
	}

	return pcbMax;
}
