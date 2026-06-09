#include "bolun/bolun_scheduler.h"
#include "bolun/bolun_result.h"

#include <stdlib.h>
#include <string.h>

int bolun_scheduler_init(BolunScheduler *scheduler) {
    if (scheduler == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }

    memset(scheduler, 0, sizeof(*scheduler));
    scheduler->next_task_id = 1;
    scheduler->total_tasks = 0;
    scheduler->current_time_ms = 0;
    scheduler->context_switches = 0;
    scheduler->ready_queue = NULL;
    scheduler->running_task = NULL;

    return BOLUN_OK;
}

uint32_t bolun_scheduler_create_task(BolunScheduler *scheduler,
                                      const char *task_name,
                                      void (*task_func)(void *),
                                      void *task_arg,
                                      BolunTaskPriority priority,
                                      uint32_t time_slice_ms) {
    if (scheduler == NULL || task_func == NULL) {
        return 0;
    }

    BolunTaskControlBlock *tcb = (BolunTaskControlBlock *)malloc(sizeof(BolunTaskControlBlock));
    if (tcb == NULL) {
        return 0;
    }

    memset(tcb, 0, sizeof(*tcb));
    tcb->task_id = scheduler->next_task_id++;
    tcb->task_name = task_name;
    tcb->task_func = task_func;
    tcb->task_arg = task_arg;
    tcb->state = BOLUN_TASK_READY;
    tcb->priority = priority;
    tcb->time_slice_ms = time_slice_ms > 0 ? time_slice_ms : 10;
    tcb->cpu_time_ms = 0;
    tcb->creation_time = scheduler->current_time_ms;

    if (scheduler->ready_queue == NULL) {
        scheduler->ready_queue = tcb;
        tcb->next = NULL;
    } else {
        BolunTaskControlBlock *current = scheduler->ready_queue;
        BolunTaskControlBlock *prev = NULL;

        while (current != NULL && current->priority >= priority) {
            prev = current;
            current = current->next;
        }

        if (prev == NULL) {
            tcb->next = scheduler->ready_queue;
            scheduler->ready_queue = tcb;
        } else {
            tcb->next = current;
            prev->next = tcb;
        }
    }

    scheduler->total_tasks++;
    return tcb->task_id;
}

uint32_t bolun_scheduler_next(BolunScheduler *scheduler) {
    if (scheduler == NULL) {
        return 0;
    }

    if (scheduler->running_task != NULL && scheduler->running_task->state == BOLUN_TASK_RUNNING) {
        scheduler->running_task->state = BOLUN_TASK_READY;
        
        BolunTaskControlBlock *current = scheduler->ready_queue;
        BolunTaskControlBlock *prev = NULL;

        if (scheduler->ready_queue == scheduler->running_task) {
            scheduler->ready_queue = scheduler->running_task->next;
        } else {
            while (current != NULL && current != scheduler->running_task) {
                prev = current;
                current = current->next;
            }
            if (prev != NULL) {
                prev->next = scheduler->running_task->next;
            }
        }

        scheduler->running_task->next = NULL;
        current = scheduler->ready_queue;
        prev = NULL;

        while (current != NULL && current->priority >= scheduler->running_task->priority) {
            prev = current;
            current = current->next;
        }

        if (prev == NULL) {
            scheduler->running_task->next = scheduler->ready_queue;
            scheduler->ready_queue = scheduler->running_task;
        } else {
            scheduler->running_task->next = current;
            prev->next = scheduler->running_task;
        }
    }

    if (scheduler->ready_queue == NULL) {
        scheduler->running_task = NULL;
        return 0;
    }

    scheduler->running_task = scheduler->ready_queue;
    scheduler->ready_queue = scheduler->ready_queue->next;
    scheduler->running_task->state = BOLUN_TASK_RUNNING;
    scheduler->running_task->cpu_time_ms += scheduler->running_task->time_slice_ms;
    scheduler->context_switches++;

    return scheduler->running_task->task_id;
}

int bolun_scheduler_suspend(BolunScheduler *scheduler, uint32_t task_id) {
    if (scheduler == NULL || task_id == 0) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }

    BolunTaskControlBlock *task = bolun_scheduler_get_task(scheduler, task_id);
    if (task == NULL || task->state == BOLUN_TASK_TERMINATED) {
        return BOLUN_ERROR_NOT_FOUND;
    }

    task->state = BOLUN_TASK_SUSPENDED;
    return BOLUN_OK;
}

int bolun_scheduler_resume(BolunScheduler *scheduler, uint32_t task_id) {
    if (scheduler == NULL || task_id == 0) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }

    BolunTaskControlBlock *task = bolun_scheduler_get_task(scheduler, task_id);
    if (task == NULL || task->state != BOLUN_TASK_SUSPENDED) {
        return BOLUN_ERROR_NOT_FOUND;
    }

    task->state = BOLUN_TASK_READY;
    return BOLUN_OK;
}

int bolun_scheduler_terminate(BolunScheduler *scheduler, uint32_t task_id) {
    if (scheduler == NULL || task_id == 0) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }

    BolunTaskControlBlock *task = bolun_scheduler_get_task(scheduler, task_id);
    if (task == NULL) {
        return BOLUN_ERROR_NOT_FOUND;
    }

    task->state = BOLUN_TASK_TERMINATED;
    
    if (scheduler->ready_queue == task) {
        scheduler->ready_queue = task->next;
    } else {
        BolunTaskControlBlock *current = scheduler->ready_queue;
        while (current != NULL && current->next != task) {
            current = current->next;
        }
        if (current != NULL) {
            current->next = task->next;
        }
    }

    if (scheduler->running_task == task) {
        scheduler->running_task = NULL;
    }

    scheduler->total_tasks--;
    free(task);
    return BOLUN_OK;
}

BolunTaskControlBlock *bolun_scheduler_get_running(const BolunScheduler *scheduler) {
    if (scheduler == NULL) {
        return NULL;
    }
    return scheduler->running_task;
}

BolunTaskControlBlock *bolun_scheduler_get_task(const BolunScheduler *scheduler, uint32_t task_id) {
    if (scheduler == NULL || task_id == 0) {
        return NULL;
    }

    BolunTaskControlBlock *current = scheduler->ready_queue;
    while (current != NULL) {
        if (current->task_id == task_id) {
            return current;
        }
        current = current->next;
    }

    if (scheduler->running_task != NULL && scheduler->running_task->task_id == task_id) {
        return scheduler->running_task;
    }

    return NULL;
}

void bolun_scheduler_update_time(BolunScheduler *scheduler, uint32_t delta_ms) {
    if (scheduler == NULL) {
        return;
    }
    scheduler->current_time_ms += delta_ms;
}

void bolun_scheduler_destroy(BolunScheduler *scheduler) {
    if (scheduler == NULL) {
        return;
    }

    BolunTaskControlBlock *current = scheduler->ready_queue;
    while (current != NULL) {
        BolunTaskControlBlock *next = current->next;
        free(current);
        current = next;
    }

    if (scheduler->running_task != NULL) {
        free(scheduler->running_task);
    }

    memset(scheduler, 0, sizeof(*scheduler));
}
