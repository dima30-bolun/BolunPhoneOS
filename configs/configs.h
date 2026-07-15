#ifndef BOLUN_CONFIGS_H
#define BOLUN_CONFIGS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BolunCpuFamily {
    BOLUN_CPU_ARMV7 = 1,
    BOLUN_CPU_ARM64 = 2,
    BOLUN_CPU_X86 = 3,
    BOLUN_CPU_X86_64 = 4
} BolunCpuFamily;

typedef struct BolunCONFIGSState {
    BolunCpuFamily cpu;
    uint32_t initialized;
    uint32_t generation;
    char name[32];
} BolunCONFIGSState;

int bolun_configs_init(BolunCONFIGSState *state, BolunCpuFamily cpu);
const char *bolun_configs_describe(void);
int bolun_configs_health(const BolunCONFIGSState *state);
int bolun_configs_route(BolunCONFIGSState *state, const char *command);

#ifdef __cplusplus
}
#endif

#endif
