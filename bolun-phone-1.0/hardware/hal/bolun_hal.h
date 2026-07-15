#ifndef BOLUN_HAL_H
#define BOLUN_HAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BolunHalClass { BOLUN_HAL_DISPLAY = 1, BOLUN_HAL_AUDIO, BOLUN_HAL_CAMERA, BOLUN_HAL_SENSOR, BOLUN_HAL_STORAGE, BOLUN_HAL_POWER, BOLUN_HAL_USB, BOLUN_HAL_BLUETOOTH, BOLUN_HAL_WIFI, BOLUN_HAL_RADIO, BOLUN_HAL_NFC } BolunHalClass;
typedef struct BolunHalDevice { BolunHalClass type; const char *name; uint32_t unit; uint32_t flags; int (*probe)(void *ctx); int (*start)(void *ctx); int (*stop)(void *ctx); int (*ioctl)(void *ctx, uint32_t request, void *data); } BolunHalDevice;
int bolun_hal_validate(const BolunHalDevice *device);

#ifdef __cplusplus
}
#endif

#endif
