#ifndef BOLUN_STORAGE_H
#define BOLUN_STORAGE_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunSTORAGEState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunSTORAGEState;

int bolun_storage_init(BolunSTORAGEState *state, BolunCpuFamily cpu);
const char *bolun_storage_describe(void);
int bolun_storage_health(const BolunSTORAGEState *state);
int bolun_storage_route(BolunSTORAGEState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
