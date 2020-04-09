/*
 * queue.c
 *
 *  Created on: Feb 20, 2020
 *      Author: Andy  Hsu
 */

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

struct node_t* create_queue(struct info_t* message, int size) {
    if(size <= 0){
        return NULL;         // Corner case
    }
	struct node_t * temp, * prev, * head;
	int i;
    if(size != 1){
        for(i = 0; i < size; i++){
            // dynamically allocate one node
            temp = create_new_node(message);

            // increment address pointer
            message++;

            if(i != 0)
                prev->next = temp;                  // points previous node to current
            else
                head = temp;                        // first interaction just made head
            prev = temp;

        }
    }else{
        head = create_new_node(message);
    }

    return head;
}

struct node_t* create_new_node(struct info_t* message) {
	// declare node
	struct node_t * temp;

    // allocate memory in heap
    temp = (struct node_t*) malloc(sizeof(struct node_t));

	// set message in node
	temp->message = message;

	// point to NULL
	temp->next = NULL;

    // return null so it compiles
    return temp;
}

struct info_t* peek(struct node_t** head) {
    // return top of queues message
    return (*head)->message;                           // returns message of head
}

void pop(struct node_t** head) {
    struct node_t * temp = *head;
    *head = (*head)->next;               // move head to next in list
    temp->next = NULL;                  // remove link of node popped
    free(temp->message);                // free the pointer allocated for info
    free(temp);
    return;
}

void push(struct node_t** head, struct info_t* message) {
    if(*head != NULL){
        struct node_t * temp = * head;
        struct node_t * new = create_new_node(message);
        while(temp->next != NULL){       // end of list
            temp = temp->next;
        }

        temp->next = new;               //set new at end of list
    }else{
        *head = create_new_node(message);
    }
}

int is_empty(struct node_t** head) {
    if(*head == NULL)                   // if the pointer points to NULL, queue is empty
        return 1;
    else
        return 0;
}

void empty_queue(struct node_t** head) {
    struct node_t * temp, * prev;
    temp = *head;
    while (temp != NULL)
    {
        prev = temp;
        temp = temp->next;
        free(prev->message);
        free(prev);                             // traverse through queue freeing node before
    }
    temp = NULL;
    prev = NULL;
    return;
}
