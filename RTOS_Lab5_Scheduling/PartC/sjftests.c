#include <stdlib.h>
#include "ctest.h"
#include "sjf.h"
#include <time.h>

CTEST_DATA(shortestjobfirst) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(shortestjobfirst) {
    int execution[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);

    init(data->task, execution, data->size);
    shortest_job_first(data->task, data->size);
}

CTEST2(shortestjobfirst, test_process) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }
}

CTEST2(shortestjobfirst, test_process0) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i + 1, (int)data->task[i].execution_time);
    }
}

CTEST2(shortestjobfirst, test_process1) {
    int waiting[] = {0, 1, 3};
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(waiting[i], (int)data->task[i].waiting_time);
    }
}

CTEST2(shortestjobfirst, test_process2) {
    int turnaround[] = {1, 3, 6};
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(turnaround[i], (int)data->task[i].turnaround_time);
    }
}

CTEST2(shortestjobfirst, test_process3) {
    ASSERT_EQUAL(4 / 3, calculate_average_wait_time(data->task, data->size));
}

CTEST2(shortestjobfirst, test_process4) {
    ASSERT_EQUAL(10 / 3, calculate_average_turn_around_time(data->task, data->size));
}


CTEST_DATA(shortestjobfirst1) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(shortestjobfirst1) {
    int execution[] = {3, 2, 1};
    data->size = sizeof(execution) / sizeof(execution[0]);

    init(data->task, execution, data->size);
    shortest_job_first(data->task, data->size);
}

CTEST2(shortestjobfirst1, test_process) {
    int index = 0;
    for(int i = 2; i > 0; i--)
    {
        ASSERT_EQUAL(i, (int)data->task[index++].process_id);
    }
}   

CTEST2(shortestjobfirst1, test_process1) {
    for(int i = 0; i < data->size; i++)
    {
        ASSERT_EQUAL(i + 1, (int)data->task[i].execution_time);
    }
}   

CTEST2(shortestjobfirst1, test_process2) {
    int waiting[] = {0, 1, 3};
    for(int i = 0; i < data->size; i++)
    {
        ASSERT_EQUAL(waiting[i], (int)data->task[i].waiting_time);
    }
}   

CTEST2(shortestjobfirst1, test_process3) {
    int turnaround[] = {1, 3, 6};
    for(int i = 0; i < data->size; i++)
    {
        ASSERT_EQUAL(turnaround[i], (int)data->task[i].turnaround_time);
    }
} 

CTEST2(shortestjobfirst1, test_process4) {
    ASSERT_EQUAL(calculate_average_wait_time(data->task, data->size), 4 / data->size);
}   

CTEST2(shortestjobfirst1, test_process5) {
    ASSERT_EQUAL(calculate_average_turn_around_time(data->task, data->size), 10 / data->size);
}   
// TODO add additional tests to help debug
