#include "sjf.h"
#include "queue.h"
#include <stdio.h>

void init(struct task_t *task, int *execution, int size) 
{
    for(int i = 0; i < size; i++)
    {
        task[i].process_id = i;
        task[i].execution_time = execution[i];
    }
}

void shortest_job_first(struct task_t *task, int size) {
    // Hints:
    // 1. Create Queue based on the task array in the correct order
    // 2. Process each "task" until completion
    // 3. You can process by popping items from the queue
    struct node_t * head = create_queue(task, size);
    struct node_t * deletePtr;
    for(int i = 0; i < size; i++)
    {
        head->task->waiting_time = (!i) ? 0 : task[i - 1].turnaround_time;
        head->task->turnaround_time = head->task->waiting_time + head->task->execution_time;
        task[i].waiting_time = head->task->waiting_time;
        task[i].turnaround_time = head->task->turnaround_time;
        deletePtr = head;
        head = head->next;
        pop(&deletePtr);
    }
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