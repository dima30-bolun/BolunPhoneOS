#ifndef BOLUN_APPS_H
#define BOLUN_APPS_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunAPPSState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunAPPSState;

int bolun_apps_init(BolunAPPSState *state, BolunCpuFamily cpu);
const char *bolun_apps_describe(void);
int bolun_apps_health(const BolunAPPSState *state);
int bolun_apps_route(BolunAPPSState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
