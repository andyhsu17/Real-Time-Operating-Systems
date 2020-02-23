#include "priority.h"
#include "queue.h"
#include <stdio.h>

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

void init(struct task_t *task, int *execution, int *priority, int size) 
{
    for(int i = 0; i < size; i++)
    {
        task[i].process_id = i;
        task[i].execution_time = execution[i];
        task[i].priority = priority[i];
        task[i].left_to_execute = execution[i];
    }
}

void priority_schedule(struct task_t *task, int size) 
{
    // Hints:
    // 1. Create Queue based on the task array in the correct order
    // 2. Each task can be processed for a time interval of 1 (i.e quantum time of 1)
    // 3. You can process by pushing and popping items from the queue
    // 4. You must recalculate the priorities after every turn
    struct node_t * temphead = create_queue(task, size);

    int i = 0;
    int index = 0;
    const int quantum = 1;
    int timeTotal = 0;
    while(temphead != NULL)
    {
            temphead->task->waiting_time = timeTotal;
            temphead->task->turnaround_time = temphead->task->waiting_time + temphead->task->execution_time;
            if(temphead->task->execution_time < quantum)
            {
                temphead->task->left_to_execute -= temphead->task->execution_time;
                timeTotal += temphead->task->execution_time;
            }
            else
            {
                temphead->task->left_to_execute -= quantum;
                timeTotal += quantum;
            }
            task[0].waiting_time = temphead->task->waiting_time;
            task[0].turnaround_time = temphead->task->turnaround_time;
            if(temphead->task->left_to_execute <= 0)            // if task is done completing
            {
                temphead->task->left_to_execute = 0;
            }                                            // task still has remaining time to execute, move to end of queue
            task[0].left_to_execute = temphead->task->left_to_execute;
            if(task[0].execution_time == timeTotal) task[0].priority *= 4;
            if(task[0].left_to_execute == timeTotal) task[0].priority *= 2;
            sortPriority(task, size);
            while(temphead != NULL) pop(&temphead);
            temphead = create_queue(task, size);
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