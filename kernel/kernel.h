#ifndef BOLUN_KERNEL_H
#define BOLUN_KERNEL_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunKERNELState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunKERNELState;

int bolun_kernel_init(BolunKERNELState *state, BolunCpuFamily cpu);
const char *bolun_kernel_describe(void);
int bolun_kernel_health(const BolunKERNELState *state);
int bolun_kernel_route(BolunKERNELState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
