#include "rr.h"
#include "queue.h"
#include <stdio.h>

void init(struct task_t *task, int *execution, int size) 
{
    for(int i = 0; i < size; i++)
    {
        task[i].process_id = i;
        task[i].execution_time = execution[i];
        task[i].left_to_execute = execution[i];
    }
}

void round_robin(struct task_t *task, int quantum, int size) 
{
    // Hints:
    // 1. Create Queue based on the task array
    // 2. Process each "task" in round robin fashion
    // 3. You can process by pushing and popping items from the queue
    struct node_t * head = create_queue(task, size);
    struct node_t * temphead = head;
    int timeTotal = 0;
    int i = 0;
    while(!(is_empty(&temphead)))
    {
        if(i != task[i].process_id) return;                             // make sure process id is correct
        if(temphead->task->left_to_execute == quantum)                // case where there is exactly that amount of time left
        {
        
            if(temphead->task->left_to_execute != temphead->task->execution_time)
            {
                temphead->task->waiting_time = timeTotal - (temphead->task->execution_time - temphead->task->left_to_execute);
            }
            else
            {
                temphead->task->waiting_time = timeTotal;
            }
            temphead->task->turnaround_time = temphead->task->waiting_time + temphead->task->execution_time;
            timeTotal += temphead->task->left_to_execute;
            temphead->task->left_to_execute = 0;
        }
        else if(temphead->task->left_to_execute < quantum)
        {
            
            if(temphead->task->left_to_execute != temphead->task->execution_time)
            {
                temphead->task->waiting_time = timeTotal - (temphead->task->execution_time - temphead->task->left_to_execute);
            }
            else
            {
                temphead->task->waiting_time = timeTotal;
            }
            temphead->task->turnaround_time = temphead->task->waiting_time + temphead->task->execution_time;
            timeTotal += temphead->task->left_to_execute;
            temphead->task->left_to_execute = 0;
        }
        else
        {
            temphead->task->waiting_time = timeTotal;
            temphead->task->turnaround_time = temphead->task->waiting_time;
            temphead->task->left_to_execute -= quantum;
            timeTotal += quantum;
        }
        task[i].waiting_time = temphead->task->waiting_time;                    // now update the task array so we can delete the linked list
        task[i].execution_time = temphead->task->execution_time;
        task[i].turnaround_time = temphead->task->turnaround_time;
        task[i].left_to_execute = temphead->task->left_to_execute;
        struct node_t * deleteaddr = temphead;                              // create new pointer so it can be freed
         if(temphead->task->left_to_execute > 0)
            push(&temphead, temphead->task);
        pop(&temphead);    
        temphead = temphead->next;                                          // increment temphead
        i++;        // increment i for addressing
        
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