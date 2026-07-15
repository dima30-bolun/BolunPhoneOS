#ifndef BOLUN_SECURITY_H
#define BOLUN_SECURITY_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunSECURITYState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunSECURITYState;

int bolun_security_init(BolunSECURITYState *state, BolunCpuFamily cpu);
const char *bolun_security_describe(void);
int bolun_security_health(const BolunSECURITYState *state);
int bolun_security_route(BolunSECURITYState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
