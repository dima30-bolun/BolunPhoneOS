#ifndef BOLUN_TESTS_H
#define BOLUN_TESTS_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunTESTSState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunTESTSState;

int bolun_tests_init(BolunTESTSState *state, BolunCpuFamily cpu);
const char *bolun_tests_describe(void);
int bolun_tests_health(const BolunTESTSState *state);
int bolun_tests_route(BolunTESTSState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
