#ifndef BOLUN_SERVICES_H
#define BOLUN_SERVICES_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunSERVICESState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunSERVICESState;

int bolun_services_init(BolunSERVICESState *state, BolunCpuFamily cpu);
const char *bolun_services_describe(void);
int bolun_services_health(const BolunSERVICESState *state);
int bolun_services_route(BolunSERVICESState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
