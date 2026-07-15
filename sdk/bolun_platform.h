#ifndef BOLUN_PLATFORM_H
#define BOLUN_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BolunCpuFamily {
    BOLUN_CPU_ARMV7 = 1,
    BOLUN_CPU_ARM64 = 2,
    BOLUN_CPU_X86 = 3,
    BOLUN_CPU_X86_64 = 4
} BolunCpuFamily;

#ifdef __cplusplus
}
#endif

#endif
