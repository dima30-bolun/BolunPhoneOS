#ifndef BOLUN_SHELL_H
#define BOLUN_SHELL_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunSHELLState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunSHELLState;

int bolun_shell_init(BolunSHELLState *state, BolunCpuFamily cpu);
const char *bolun_shell_describe(void);
int bolun_shell_health(const BolunSHELLState *state);
int bolun_shell_route(BolunSHELLState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
