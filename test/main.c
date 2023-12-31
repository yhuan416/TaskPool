#include <stdio.h>

#include "TaskPool.h"

#include "osal.h"

void *Task(void *data)
{
    int tid = osal_task_self();

    while (1)
    {
        printf("Task: %d, uptime: %ld\n", tid, osal_uptime());
        osal_task_delay_ms(1000);

        if (*(uint32_t *)data == 1)
            break;
    }

    printf("tid: %d exit\n", tid);

    return NULL;
}

void Notify(void *data)
{
    printf("Notify\n");

    uint32_t *flag = (uint32_t *)data;

    *flag = 1;
}

int main(int argc, char const *argv[])
{
    uint32_t flag = 0;

    TaskConfig config = {
        .stackSize = 1024,
        .priority = 0,
        .flags = 0
    };

    TaskPool pool = TaskPool_Create("test", 2, &config);

    TaskPool_SetData(pool, &flag);

    TaskPool_Start(pool, Task, Notify);

    osal_task_delay(5);

    TaskPool_Release(pool);

    return 0;
}
