#ifndef BOLUN_BOOT_H
#define BOLUN_BOOT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *bolun_boot_banner(void);
uint64_t bolun_cpu_probe(void);
void bolun_interrupt_stub(void);

#ifdef __cplusplus
}
#endif

#endif
