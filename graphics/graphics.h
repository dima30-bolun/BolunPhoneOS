#ifndef BOLUN_GRAPHICS_H
#define BOLUN_GRAPHICS_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunGRAPHICSState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunGRAPHICSState;

int bolun_graphics_init(BolunGRAPHICSState *state, BolunCpuFamily cpu);
const char *bolun_graphics_describe(void);
int bolun_graphics_health(const BolunGRAPHICSState *state);
int bolun_graphics_route(BolunGRAPHICSState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
