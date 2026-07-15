#ifndef BOLUN_NET_H
#define BOLUN_NET_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunNETState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunNETState;

int bolun_net_init(BolunNETState *state, BolunCpuFamily cpu);
const char *bolun_net_describe(void);
int bolun_net_health(const BolunNETState *state);
int bolun_net_route(BolunNETState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
