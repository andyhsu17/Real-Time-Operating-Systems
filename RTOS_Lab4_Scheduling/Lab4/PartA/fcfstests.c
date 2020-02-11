#include <stdlib.h>
#include "ctest.h"
#include "fcfs.h"

// Note: the name in the first parameter slot must match all tests in that group
CTEST_DATA(firstcomefirstserved) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(firstcomefirstserved) {
    int execution[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);
    init(data->task, execution, data->size);
    first_come_first_served(data->task, data->size);
}

CTEST2(firstcomefirstserved, test_process) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }
}

// TODO add additional tests to help debug
