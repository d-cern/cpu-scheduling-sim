#ifndef LINKEDLIST_H
#define LINKEDLIST_H


//------- STRUCTS -------//
/*
 * This type defines the nodes of a Linkedlist_st
 * that store the informatin for a Process Control
 * Block (PCB).
 */
typedef struct PCB_st {
	int procId;
	int procPrio;
	int cpuBurst;
	int myReg[8];
	int queueEnterClock, waitingTime;
	struct PCB_st *next;
} PCB_st;

/*
 * This type defines a linked list of PCB_st. It
 * stores pointers to the head and tail of the
 * linked list.
 */
typedef struct Linkedlist_st {
	PCB_st *head;
	PCB_st *tail;
} Linkedlist_st;


//------ FUNCTIONS ------//
/*
 * Allocates memory for a pcb node and initializes
 * its members. Returns the node.
 */
PCB_st *NewPCBnode(int id, int prio, int burst);

/*
 * Allocates memory for a linked list and returns
 * the list.
 */
Linkedlist_st *NewLinkedList();

/*
 * Frees memory assigned to passed list.
 */
void FreeLinkedList(Linkedlist_st *list);

/*
 * Adds node to the end of list. If list is empty,
 * adds to beginning.
 */
void Enlist(Linkedlist_st *list, PCB_st *node);

/*
 * Removes head node from list and returns a pointer
 * to it.
 */
PCB_st *Delist(Linkedlist_st *list);

/*
 * Returns 1 if list is empty, 0 if not empty.
 */
int LinkedListIsEmpty(Linkedlist_st *list);

/*
 * Returns the number of nodes in list.
 */
int LinkedListLength(Linkedlist_st *list);

/*
 * Returns the node at index, where 
 * list->head == index(0). Returns NULL if
 * index > LinkedListLength(list) || index < 0.
 */
PCB_st *GetLinkedListNode(Linkedlist_st *list, int index);

PCB_st *PopMinCPUburst(Linkedlist_st *list);

PCB_st *PopHighestPrio(Linkedlist_st *list);

#endif
