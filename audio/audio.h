#ifndef BOLUN_AUDIO_H
#define BOLUN_AUDIO_H

#include <stddef.h>
#include <stdint.h>

#include "bolun_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BolunAUDIOState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunAUDIOState;

int bolun_audio_init(BolunAUDIOState *state, BolunCpuFamily cpu);
const char *bolun_audio_describe(void);
int bolun_audio_health(const BolunAUDIOState *state);
int bolun_audio_route(BolunAUDIOState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
