#ifndef BOLUN_POWER_H
#define BOLUN_POWER_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunPOWERState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunPOWERState;

int bolun_power_init(BolunPOWERState *state, BolunCpuFamily cpu);
const char *bolun_power_describe(void);
int bolun_power_health(const BolunPOWERState *state);
int bolun_power_route(BolunPOWERState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
