#include "fcfs.h"
#include "queue.h"
#include "ctest.h"
#include <stdio.h>
#include <string.h>

void init(struct task_t *task, int *execution, int size) 
{
    for(int i = 0; i < size; i++)
    {
        task[i].process_id = i;
        task[i].execution_time = execution[i];
    }
}

void first_come_first_served(struct task_t *task, int size) 
{
    struct node_t * head = create_queue(task, size);
    struct node_t * temphead = head;
    for(int i = 0; i < size; i++)
    {
        temphead->task->execution_time = task[i].execution_time;        // get the execution time from the task array (these should be constant)
        ASSERT_EQUAL(i, (int)task[i].process_id);                            // make sure that the task id is equivalent to the iterator in the for loop
        if(i == 0)                  //waiting time is 0
        {
            temphead->task->waiting_time = 0;
        }
        else 
        {
            temphead->task->waiting_time = task[i - 1].turnaround_time;
        }
        temphead->task->turnaround_time = temphead->task->waiting_time + temphead->task->execution_time;    // get turnaround time which is the waiting time plus execution time
        task[i].waiting_time = temphead->task->waiting_time;                                        // now update the task array
        task[i].turnaround_time = temphead->task->turnaround_time;
        struct node_t * deleteaddr = temphead;
        temphead = temphead->next;
        pop(&deleteaddr);
    }
    float waitT = calculate_average_wait_time(task, size);              // calculate wait times and turnaround times
    float turnT = calculate_average_turn_around_time(task, size);
    // memset(task, (int)NULL, size * sizeof(struct task_t));               // clear the task queue
}

float calculate_average_wait_time(struct task_t *task, int size) 
{
    float sum = 0;
    for(int i = 0; i < size; i++)
    {
        sum += task[i].waiting_time;
    }
    return sum / size;
}

float calculate_average_turn_around_time(struct task_t *task, int size) 
{
    float sum = 0;
    for(int i = 0; i < size; i++)
    {
        sum+= task[i].turnaround_time;
    }
    return sum / size;
}