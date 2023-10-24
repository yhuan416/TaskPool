#ifndef _TASKPOOL_H_
#define _TASKPOOL_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct TaskConfig {
    uint32_t stackSize;
    uint16_t priority;
    uint8_t flags;
} TaskConfig;

typedef void *TaskPool;

typedef void *(*TaskPool_Task)(void *data);
typedef void (*TaskPool_Notify)(void *data);

TaskPool TaskPool_Create(const char *name, uint8_t size, TaskConfig *configs);

int32_t TaskPool_Start(TaskPool pool, TaskPool_Task task, TaskPool_Notify notify);

TaskPool TaskPool_Reference(TaskPool pool);

int32_t TaskPool_Release(TaskPool pool);

void TaskPool_SetData(TaskPool pool, void *data);

void *TaskPool_GetData(TaskPool pool);

#endif // !_TASKPOOL_H_
