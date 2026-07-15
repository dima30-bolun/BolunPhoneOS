#ifndef BOLUN_FS_H
#define BOLUN_FS_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunFSState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunFSState;

int bolun_fs_init(BolunFSState *state, BolunCpuFamily cpu);
const char *bolun_fs_describe(void);
int bolun_fs_health(const BolunFSState *state);
int bolun_fs_route(BolunFSState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
