#ifndef BOLUN_MM_H
#define BOLUN_MM_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunMMState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunMMState;

int bolun_mm_init(BolunMMState *state, BolunCpuFamily cpu);
const char *bolun_mm_describe(void);
int bolun_mm_health(const BolunMMState *state);
int bolun_mm_route(BolunMMState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
