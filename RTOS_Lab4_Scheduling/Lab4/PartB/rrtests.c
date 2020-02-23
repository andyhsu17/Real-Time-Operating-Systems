#include <stdlib.h>
#include "ctest.h"
#include "rr.h"


CTEST_DATA(roundrobin) 
{
    struct task_t task[3];
    int size;
};

CTEST_SETUP(roundrobin) 
{
    int execution[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);
    int quantum = 2;

    init(data->task, execution, data->size);
    round_robin(data->task, quantum, data->size);
}

CTEST2(roundrobin, test_process) 
{
    for (int i = 0; i < data->size; i++) 
    {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }

}


CTEST2(roundrobin, test_process1) 
{
    int execution[] = {1, 2, 3};            
    for (int i = 0; i < data->size; i++) 
    {
        ASSERT_EQUAL(execution[i], (int)data->task[i].execution_time);
    }
}

CTEST2(roundrobin, test_process2) 
{
    int waiting[] = {0, 1, 3};            // waiting time should be previous tasks execution time
    for (int i = 0; i < data->size; i++) 
    {
        ASSERT_EQUAL(waiting[i], (int)data->task[i].waiting_time);
    }
}

CTEST2(roundrobin, test_process3) 
{
    int turnaround[] = {1, 3, 6};            // turnaround time should be tasks execution time + waiting time
    for (int i = 0; i < data->size; i++) 
    {
        ASSERT_EQUAL(turnaround[i], (int)data->task[i].turnaround_time);
    }
}

CTEST2(roundrobin, test_process4) 
{
    int timeleft[] = {0, 0, 0};            // turnaround time should be tasks execution time + waiting time
    for (int i = 0; i < data->size; i++) 
    {
        ASSERT_EQUAL(timeleft[i], (int)data->task[i].left_to_execute);
    }
}

CTEST2(roundrobin, test_process5) 
{
    float expectedAverage = (0 + 1 + 3) / data->size;            
    float average = calculate_average_wait_time(data->task, data->size);
    ASSERT_EQUAL(expectedAverage, average);
}

CTEST2(roundrobin, test_process6) 
{
    float expectedAverage = (1 + 3 + 6) / data->size;            // total turnaround time divided by number tasks
    float average = calculate_average_turn_around_time(data->task, data->size);
    ASSERT_EQUAL(expectedAverage, average);
}


