#include <stdlib.h>
#include "ctest.h"
#include "rr.h"

CTEST_DATA(roundrobin) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(roundrobin) {
    int execution[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);
    int quantum = 2;

    init(data->task, execution, data->size);
    round_robin(data->task, quantum, data->size);
}

CTEST2(roundrobin, test_process) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }
}

// TODO add additional tests to help debug