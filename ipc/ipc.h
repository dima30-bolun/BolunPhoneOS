#ifndef BOLUN_IPC_H
#define BOLUN_IPC_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunIPCState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunIPCState;

int bolun_ipc_init(BolunIPCState *state, BolunCpuFamily cpu);
const char *bolun_ipc_describe(void);
int bolun_ipc_health(const BolunIPCState *state);
int bolun_ipc_route(BolunIPCState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
