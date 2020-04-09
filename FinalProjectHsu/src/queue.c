/*
 * queue.c
 *
 *  Created on: Feb 20, 2020
 *      Author: Andy Hsu
 */
#include "queue.h"

struct node_t* create_queue(struct info_t* msg, int size)
{
	struct node_t* temp, *prev, *head;
	if(size > 1)
	{
		for(int i = 0; i < size; i++)
		{
			// allocate each node
			temp = create_new_node(msg);

			if(i != 0)
				prev->next = temp;
			else
				head = temp;
			prev = temp;

			//increment in msg array
			msg++;
		}

		return head;
	}
	else if(size == 1)
	{
		temp = create_new_node(msg);
		head = temp;

		return head;
	}
	else
	{
		return NULL;
	}
}

struct node_t* create_new_node(struct info_t* msg) {

	struct node_t* temp;

	temp = (struct node_t*)malloc(sizeof(struct node_t));

	// make new_node msgs equal to inputed msg
	temp->msg = msg;

	temp->next = NULL;

	// return null so it compiles
	return temp;
}

struct info_t* peek(struct node_t** head) {
	return (*head)->msg;
}

void pop(struct node_t** head) {
	if(!is_empty(head))
	{
		struct node_t* top;
		top = *head;
		*head = (*head)->next;
		top->next = NULL;
	}
}

void push(struct node_t** head, struct info_t* msg) {
	struct node_t* temp = *head;
	struct node_t* new_node = create_new_node(msg);

	while(temp->next != NULL)
	{
		temp = temp->next;
	}
	temp->next = new_node;
}

int is_empty(struct node_t** head) {
	// If *head is NULL then it is empty
	if(*head != NULL)
		return 0;
	else
		return 1;
}

void empty_queue(struct node_t** head) {
	struct node_t* temp, * prev;
	temp = *head;

	while(temp != NULL)
	{
		prev = temp;
		temp = temp->next;
		prev->next = NULL;
		free(prev);
	}
}
