#ifndef BOLUN_SCHEDULER_H
#define BOLUN_SCHEDULER_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunSCHEDULERState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunSCHEDULERState;

int bolun_scheduler_init(BolunSCHEDULERState *state, BolunCpuFamily cpu);
const char *bolun_scheduler_describe(void);
int bolun_scheduler_health(const BolunSCHEDULERState *state);
int bolun_scheduler_route(BolunSCHEDULERState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
