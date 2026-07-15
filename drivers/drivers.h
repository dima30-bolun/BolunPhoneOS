#ifndef BOLUN_DRIVERS_H
#define BOLUN_DRIVERS_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunDRIVERSState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunDRIVERSState;

int bolun_drivers_init(BolunDRIVERSState *state, BolunCpuFamily cpu);
const char *bolun_drivers_describe(void);
int bolun_drivers_health(const BolunDRIVERSState *state);
int bolun_drivers_route(BolunDRIVERSState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
