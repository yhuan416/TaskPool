#include "TaskPool.h"

#include "pthread.h"

// #include "task_adapter.h"
#include "ual.h"

typedef struct TaskPoolImpl {
    uint32_t stackSize; // 栈大小
    uint16_t priority;  // 优先级
    uint8_t size;       // 线程数量
    uint8_t top;        // 当前线程数量
    int8_t ref;         // 引用计数
    TaskPool_Notify notify; // 通知函数
    void *data;         // 私有数据
    taskId tasks[0];    // 线程组
} TaskPoolImpl;

TaskPool TaskPool_Create(const char *name, uint8_t size, TaskConfig *configs)
{
    TaskPoolImpl *new_pool = NULL;

    if (size == 0 || configs == NULL)
        return NULL;
    
    new_pool = (TaskPoolImpl *)malloc(sizeof(struct TaskPoolImpl) + sizeof(taskId) * size);
    if (new_pool == NULL)
        return NULL;

    memset(new_pool, 0, sizeof(struct TaskPoolImpl) + sizeof(taskId) * size);

    new_pool->stackSize = configs->stackSize;
    new_pool->priority = configs->priority;
    new_pool->size = size;
    new_pool->top = 0;
    new_pool->ref = 1;

    return (TaskPool)new_pool;
}

int32_t TaskPool_Start(TaskPool pool, TaskPool_Task task, TaskPool_Notify notify)
{
    TaskPoolImpl *pool_impl = (TaskPoolImpl *)pool;
    if (pool_impl == NULL)
        return -1;

    if (pool_impl->top >= pool_impl->size)
        return -1;

    while (pool_impl->top < pool_impl->size)  {
        taskAttr attr = {
            .name = "TaskPool",
            .stackSize = pool_impl->stackSize,
            .priority = pool_impl->priority,
        };

        taskId tid = ual_task_create(task, pool_impl->data, &attr);
        if (tid == NULL)
            return -1;

        pool_impl->tasks[pool_impl->top] = tid;
        pool_impl->top++;
    }

    pool_impl->notify = notify;

    return 0;
}

TaskPool TaskPool_Reference(TaskPool pool)
{
    TaskPoolImpl *pool_impl = (TaskPoolImpl *)pool;
    if (pool_impl == NULL)
        return NULL;
    
    pool_impl->ref++;
    return pool;
}

int32_t TaskPool_Release(TaskPool pool)
{
    TaskPoolImpl *pool_impl = (TaskPoolImpl *)pool;
    if (pool_impl == NULL)
        return -1;
    
    pool_impl->ref--;
    if (pool_impl->ref == 0)
    {
        // notify
        if (pool_impl->notify != NULL)
            pool_impl->notify(pool_impl->data);

        // 等待线程结束
        for (uint8_t i = 0; i < pool_impl->top; i++)
        {
            // pthread_join(pool_impl->tasks[i], NULL);
            ual_task_join(pool_impl->tasks[i]);
        }

        free(pool_impl);
        return 0;
    }
    return -1;
}

void TaskPool_SetData(TaskPool pool, void *data)
{
    TaskPoolImpl *pool_impl = (TaskPoolImpl *)pool;
    if (pool_impl == NULL)
        return;
    
    pool_impl->data = data;
}

void *TaskPool_GetData(TaskPool pool)
{
    TaskPoolImpl *pool_impl = (TaskPoolImpl *)pool;
    if (pool_impl == NULL)
        return NULL;

    return pool_impl->data;
}
