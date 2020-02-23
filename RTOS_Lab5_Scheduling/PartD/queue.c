#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <stdbool.h>

static void sortPriority(struct task_t * task, int size)
{
    struct task_t temptask;
    for(int i = 0; i < size; i++)                   // sort tasks by priority
    {
        for(int j = i + 1; j < size; j++)
        {
            if(task[j].priority > task[i].priority)
            {
                temptask = task[i];
                task[i] = task[j];
                task[j] = temptask;
            }
        }
    }
}

struct node_t* create_queue(struct task_t* task, int size)
{
    int tempPrio;
    int tempExecution;
    static bool initialized = false;
    struct node_t * head = NULL;   
    if(initialized)
    {
        sortPriority(task, size);
    }
    else
    {
        for(int i = 0; i < size; i++)                   // sort tasks by priority, dont swap process id
        {
            for(int j = i + 1; j < size; j++)
            {
                if(task[j].priority > task[i].priority)
                {
                    tempPrio = task[i].priority;
                    tempExecution = task[i].execution_time;
                    task[i].priority = task[j].priority;
                    task[i].execution_time = task[j].execution_time;
                    task[j].priority = tempPrio;
                    task[j].execution_time = tempExecution;
                }
            }
        }    
    }

    for(int i = 0; i < size; i++)
    {
        if(task[i].left_to_execute > 0)
        {
            if(!head)
            {
                head = create_new_node(&task[i]);
            }
            push(&head, &task[i]);
        }
    }
    initialized = true;
    return head;
}

struct node_t* create_new_node(struct task_t* task) 
{
    static struct node_t * newnode;
    newnode = malloc(sizeof(struct node_t));
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
    temphead->next = NULL;
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
    if( *head == NULL) return 1;
    else return 0;
}

void empty_queue(struct node_t** head) 
{
    while(*head != NULL)
    {
        pop(head);
    }
}


