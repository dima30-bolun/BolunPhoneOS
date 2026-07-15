#include "bolun_hal.h"

int bolun_hal_validate(const BolunHalDevice *device) {
    if (!device || !device->name || !device->probe || !device->start || !device->stop || !device->ioctl) return -1;
    return device->type >= BOLUN_HAL_DISPLAY && device->type <= BOLUN_HAL_NFC ? 0 : -1;
}
