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


// #include <stdlib.h>
// #include "ctest.h"
// #include "rr.h"

// CTEST_DATA(roundrobin) {
//     struct task_t task[3];
//     int size;
// };

// CTEST_SETUP(roundrobin) {
//     int execution[] = {1, 2, 3};
//     data->size = sizeof(execution) / sizeof(execution[0]);
//     int quantum = 2;

//     init(data->task, execution, data->size);
//     round_robin(data->task, quantum, data->size);
// }

// CTEST2(roundrobin, test_process) {
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(i, (int)data->task[i].process_id);
//     }
// }

// CTEST2(roundrobin, test_process1) {
//     int execution[] = {1, 2, 3};            // same execution array passed in
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(execution[i], (int)data->task[i].execution_time);
//     }
// }

// CTEST2(roundrobin, test_process2) {
//     int waitingTime[] = {0, 1, 3};            // waiting time should be previous tasks execution time
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(waitingTime[i], (int)data->task[i].waiting_time);
//     }
// }

// CTEST2(roundrobin, test_process3) {
//     int turnaroundTime[] = {1, 3, 6};            // turnaround time should be tasks execution time + waiting time
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(turnaroundTime[i], (int)data->task[i].turnaround_time);
//     }
// }

// CTEST2(roundrobin, test_process4) {
//     int leftToExecute[] = {0, 0, 0};            // turnaround time should be tasks execution time + waiting time
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(leftToExecute[i], (int)data->task[i].left_to_execute);
//     }
// }

// CTEST2(roundrobin, test_process5) {
//     float expectedAverage = (0 + 1 + 3) / data->size;            // total waiting time divided by number tasks
//     float average = calculate_average_wait_time(data->task, data->size);
//     ASSERT_EQUAL(expectedAverage, average);
// }

// CTEST2(roundrobin, test_process6) {
//     float expectedAverage = (1 + 3 + 6) / data->size;            // total turnaround time divided by number tasks
//     float average = calculate_average_turn_around_time(data->task, data->size);
//     ASSERT_EQUAL(expectedAverage, average);
// }
// //TODO add additional tests to help debug

// CTEST_DATA(roundrobin1) {
//     struct task_t task[4];
//     int size;
// };

// CTEST_SETUP(roundrobin1) {
//     int execution[] = {5, 2, 10, 6};
//     data->size = sizeof(execution) / sizeof(execution[0]);
//     int quantum = 3;

//     init(data->task, execution, data->size);
//     round_robin(data->task, quantum, data->size);
// }

// CTEST2(roundrobin1, test_process) {
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(i, (int)data->task[i].process_id);
//     }
// }

// CTEST2(roundrobin1, test_process1) {
//     int execution[] = {5, 2, 10, 6};            // same execution array passed in
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(execution[i], (int)data->task[i].execution_time);
//     }
// }

// CTEST2(roundrobin1, test_process2) {
//     int waitingTime[] = {8, 3, 13, 13};            // waiting time should be previous tasks execution time
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(waitingTime[i], (int)data->task[i].waiting_time);
//     }
// }

// CTEST2(roundrobin1, test_process3) {
//     int turnaroundTime[] = {13, 5, 23, 19};            // turnaround time should be tasks execution time + waiting time
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(turnaroundTime[i], (int)data->task[i].turnaround_time);
//     }
// }

// CTEST2(roundrobin1, test_process4) {
//     int leftToExecute[] = {0, 0, 0, 0};            // turnaround time should be tasks execution time + waiting time
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(leftToExecute[i], (int)data->task[i].left_to_execute);
//     }
// }

// CTEST2(roundrobin1, test_process5) {
//     float expectedAverage = (8 + 3 + 13 + 13) / data->size;            // total waiting time divided by number tasks
//     float average = calculate_average_wait_time(data->task, data->size);
//     ASSERT_EQUAL(expectedAverage, average);
// }

// CTEST2(roundrobin1, test_process6) {
//     float expectedAverage = (13 + 5 + 23 + 19) / data->size;            // total turnaround time divided by number tasks
//     float average = calculate_average_turn_around_time(data->task, data->size);
//     ASSERT_EQUAL(expectedAverage, average);
// }


// CTEST_DATA(roundrobin2) {
//     struct task_t task[5];
//     int size;
// };

// CTEST_SETUP(roundrobin2) {
//     int execution[] = {4, 3, 1, 6, 5};
//     data->size = sizeof(execution) / sizeof(execution[0]);
//     int quantum = 4;

//     init(data->task, execution, data->size);
//     round_robin(data->task, quantum, data->size);
// }

// CTEST2(roundrobin2, test_process) {
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(i, (int)data->task[i].process_id);
//     }
// }

// CTEST2(roundrobin2, test_process1) {
//     int execution[] = {4, 3, 1, 6, 5};            // same execution array passed in
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(execution[i], (int)data->task[i].execution_time);
//     }
// }

// CTEST2(roundrobin2, test_process2) {
//     int waitingTime[] = {0, 4, 7, 12, 14};            // waiting time should be previous tasks execution time
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(waitingTime[i], (int)data->task[i].waiting_time);
//     }
// }

// CTEST2(roundrobin2, test_process3) {
//     int turnaroundTime[] = {4, 7, 8, 18, 19};            // turnaround time should be tasks execution time + waiting time
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(turnaroundTime[i], (int)data->task[i].turnaround_time);
//     }
// }

// CTEST2(roundrobin2, test_process4) {
//     int leftToExecute[] = {0, 0, 0, 0, 0};            // turnaround time should be tasks execution time + waiting time
//     for (int i = 0; i < data->size; i++) {
//         ASSERT_EQUAL(leftToExecute[i], (int)data->task[i].left_to_execute);
//     }
// }

// CTEST2(roundrobin2, test_process5) {
//     float expectedAverage = (0 + 4 + 7 + 12 + 14) / data->size;            // total waiting time divided by number tasks
//     float average = calculate_average_wait_time(data->task, data->size);
//     ASSERT_EQUAL(expectedAverage, average);
// }

// CTEST2(roundrobin2, test_process6) {
//     float expectedAverage = (4 + 7 + 8 + 18 + 19) / data->size;            // total turnaround time divided by number tasks
//     float average = calculate_average_turn_around_time(data->task, data->size);
//     ASSERT_EQUAL(expectedAverage, average);
// }