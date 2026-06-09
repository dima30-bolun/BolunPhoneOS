#ifndef BOLUN_SCHEDULER_H
#define BOLUN_SCHEDULER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BOLUN_TASK_READY = 0,
    BOLUN_TASK_RUNNING = 1,
    BOLUN_TASK_BLOCKED = 2,
    BOLUN_TASK_SUSPENDED = 3,
    BOLUN_TASK_TERMINATED = 4
} BolunTaskState;

typedef enum {
    BOLUN_PRIORITY_LOW = 0,
    BOLUN_PRIORITY_NORMAL = 1,
    BOLUN_PRIORITY_HIGH = 2,
    BOLUN_PRIORITY_CRITICAL = 3
} BolunTaskPriority;

typedef struct BolunTaskControlBlock {
    uint32_t task_id;
    const char *task_name;
    BolunTaskState state;
    BolunTaskPriority priority;
    void (*task_func)(void *arg);
    void *task_arg;
    uint32_t time_slice_ms;
    uint32_t cpu_time_ms;
    uint32_t creation_time;
    struct BolunTaskControlBlock *next;
} BolunTaskControlBlock;

typedef struct BolunScheduler {
    BolunTaskControlBlock *ready_queue;
    BolunTaskControlBlock *running_task;
    uint32_t next_task_id;
    uint32_t total_tasks;
    uint32_t current_time_ms;
    uint32_t context_switches;
} BolunScheduler;

int bolun_scheduler_init(BolunScheduler *scheduler);

uint32_t bolun_scheduler_create_task(BolunScheduler *scheduler,
                                      const char *task_name,
                                      void (*task_func)(void *),
                                      void *task_arg,
                                      BolunTaskPriority priority,
                                      uint32_t time_slice_ms);

uint32_t bolun_scheduler_next(BolunScheduler *scheduler);

int bolun_scheduler_suspend(BolunScheduler *scheduler, uint32_t task_id);
int bolun_scheduler_resume(BolunScheduler *scheduler, uint32_t task_id);
int bolun_scheduler_terminate(BolunScheduler *scheduler, uint32_t task_id);

BolunTaskControlBlock *bolun_scheduler_get_running(const BolunScheduler *scheduler);
BolunTaskControlBlock *bolun_scheduler_get_task(const BolunScheduler *scheduler, uint32_t task_id);

void bolun_scheduler_update_time(BolunScheduler *scheduler, uint32_t delta_ms);
void bolun_scheduler_destroy(BolunScheduler *scheduler);

#ifdef __cplusplus
}
#endif

#endif // BOLUN_SCHEDULER_H
