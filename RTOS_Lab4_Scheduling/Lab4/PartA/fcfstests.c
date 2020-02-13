#include <stdlib.h>
#include "ctest.h"
#include "fcfs.h"

// Note: the name in the first parameter slot must match all tests in that group
CTEST_DATA(firstcomefirstserved) 
{
    struct task_t task[3];
    int size;
};

CTEST_SETUP(firstcomefirstserved) 
{
    int execution[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);
    init(data->task, execution, data->size);
    first_come_first_served(data->task, data->size);
}

CTEST2(firstcomefirstserved, test_process) 
{
    for (int i = 0; i < data->size; i++) 
    {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }

}
CTEST2(firstcomefirstserved, testprocess1)
{
    int execution[] = {1, 2, 3};                    // make sure that execution time gets passed in correctly during initialization
    for(int i = 0; i < data->size; i++)
    {
        ASSERT_EQUAL(execution[i], data->task[i].execution_time);
    }
}
CTEST2(firstcomefirstserved, testprocess2)          // make sure that waiting time for each task is calculated correctly
{
    int waiting[] = {0, 1, 3};
    for(int i = 0; i < data->size; i++)
    {
        ASSERT_EQUAL(waiting[i], data->task[i].waiting_time);

    }
}
CTEST2(firstcomefirstserved, testprocess3)          // make sure that turnaround time is calculated correctly
{
    int turnaround[] = {1, 3, 6};
    for(int i = 0; i < data->size; i++)
    {
        ASSERT_EQUAL(turnaround[i], data->task[i].turnaround_time);

    }
}

CTEST2(firstcomefirstserved, testprocess4)          // make sure that turnaround time is calculated correctly
{
    float avgwait = calculate_average_wait_time(data->task, 3);
    ASSERT_EQUAL(avgwait, 4 / data->size);
}


CTEST2(firstcomefirstserved, testprocess5)          // make sure that turnaround time is calculated correctly
{
   float avgturn = calculate_average_turn_around_time(data->task, 3);
   ASSERT_EQUAL(avgturn, 10 / data->size);
}

