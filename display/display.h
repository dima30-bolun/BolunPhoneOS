#ifndef BOLUN_DISPLAY_H
#define BOLUN_DISPLAY_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunDISPLAYState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunDISPLAYState;

int bolun_display_init(BolunDISPLAYState *state, BolunCpuFamily cpu);
const char *bolun_display_describe(void);
int bolun_display_health(const BolunDISPLAYState *state);
int bolun_display_route(BolunDISPLAYState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
