#ifndef BOLUN_SDK_H
#define BOLUN_SDK_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunSDKState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunSDKState;

int bolun_sdk_init(BolunSDKState *state, BolunCpuFamily cpu);
const char *bolun_sdk_describe(void);
int bolun_sdk_health(const BolunSDKState *state);
int bolun_sdk_route(BolunSDKState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
