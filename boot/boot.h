#ifndef BOLUN_BOOT_H
#define BOLUN_BOOT_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunBOOTState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunBOOTState;

int bolun_boot_init(BolunBOOTState *state, BolunCpuFamily cpu);
const char *bolun_boot_describe(void);
int bolun_boot_health(const BolunBOOTState *state);
int bolun_boot_route(BolunBOOTState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
