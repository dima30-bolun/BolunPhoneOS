#ifndef BOLUN_INPUT_H
#define BOLUN_INPUT_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunINPUTState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunINPUTState;

int bolun_input_init(BolunINPUTState *state, BolunCpuFamily cpu);
const char *bolun_input_describe(void);
int bolun_input_health(const BolunINPUTState *state);
int bolun_input_route(BolunINPUTState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
