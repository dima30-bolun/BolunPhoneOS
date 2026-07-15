#ifndef BOLUN_HAL_H
#define BOLUN_HAL_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunHALState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunHALState;

int bolun_hal_init(BolunHALState *state, BolunCpuFamily cpu);
const char *bolun_hal_describe(void);
int bolun_hal_health(const BolunHALState *state);
int bolun_hal_route(BolunHALState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
