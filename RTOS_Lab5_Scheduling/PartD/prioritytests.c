#include <stdlib.h>
#include "ctest.h"
#include "priority.h"

CTEST_DATA(priority) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(priority) {
    int execution[] = {1, 2, 3};
    int priority[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);
    init(data->task, execution, priority, data->size);
    priority_schedule(data->task, data->size);
}

CTEST2(priority, test_process) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }
}

CTEST2(priority, test_process1) {
    int execution[] = {3, 2, 1};
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(execution[i], (int)data->task[i].execution_time);
    }
}

CTEST2(priority, test_process2) {
    int priority[] = {12, 2, 1};
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(priority[i], (int)data->task[i].priority);
    }
}

CTEST2(priority, test_process3) {
    int waiting[] = {6, 5, 6};
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(waiting[i], (int)data->task[i].waiting_time);
    }
}

CTEST2(priority, test_process4) {
    int turnaround[] = {7, 7, 7};
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(turnaround[i], (int)data->task[i].turnaround_time);
    }
}

CTEST2(priority, test_process5) {
    ASSERT_EQUAL(17 / data->size, calculate_average_wait_time(data->task, data->size));
}
CTEST2(priority, test_process6) {
    ASSERT_EQUAL(21 / data->size, calculate_average_turn_around_time(data->task, data->size));
}

// CTEST2(priority, test_process7) {

// }
