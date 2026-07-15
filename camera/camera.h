#ifndef BOLUN_CAMERA_H
#define BOLUN_CAMERA_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunCAMERAState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunCAMERAState;

int bolun_camera_init(BolunCAMERAState *state, BolunCpuFamily cpu);
const char *bolun_camera_describe(void);
int bolun_camera_health(const BolunCAMERAState *state);
int bolun_camera_route(BolunCAMERAState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
