#ifndef BOLUN_FRAMEWORK_H
#define BOLUN_FRAMEWORK_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunFRAMEWORKState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunFRAMEWORKState;

int bolun_framework_init(BolunFRAMEWORKState *state, BolunCpuFamily cpu);
const char *bolun_framework_describe(void);
int bolun_framework_health(const BolunFRAMEWORKState *state);
int bolun_framework_route(BolunFRAMEWORKState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
