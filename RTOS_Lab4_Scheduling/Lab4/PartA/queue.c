#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

struct node_t* create_queue(struct task_t* task, int size)
{
    struct node_t * head = create_new_node(&task[0]);
    for(int i = 1; i < size; i++)
    {
        push(&head, &task[i]);
    }
    return head;
}

struct node_t* create_new_node(struct task_t* task) 
{
    struct node_t * newnode = malloc(sizeof(struct node_t));
    newnode->task = task;
    newnode->next = NULL;
    return newnode;
}

struct task_t* peek(struct node_t** head)
{
    if(*head == NULL) return NULL;
    return (*head)->task;
}

void pop(struct node_t** head)
{
    if(*head == NULL) return;
    struct node_t * temphead = *head;
    (*head) = (*head)->next; 
    free(temphead);
}

void push(struct node_t** head, struct task_t* task) 
{
    struct node_t * tail = create_new_node(task);
    struct node_t * temphead = *head;
    while(temphead->next != NULL)
    {
        temphead = temphead->next;
    }
    temphead->next = tail;
}

int is_empty(struct node_t** head) 
{
    if((*head)->task == NULL || *head == NULL) return 1;
    else return 0;
}

void empty_queue(struct node_t** head) 
{
    while(*head != NULL)
    {
        pop(head);
    }
}
