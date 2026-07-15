#include "boot.h"

#include <assert.h>
#include <string.h>

int main(void) {
    BolunBOOTState state;
    assert(bolun_boot_init(&state, BOLUN_CPU_ARMV7) == 0);
    assert(bolun_boot_health(&state) == 1);
    assert(strstr(bolun_boot_describe(), "Bolun Phone 1.0") != 0);
    assert(bolun_boot_route(&state, "manifest-check") > 1);
    assert(bolun_boot_health(&state) == 2);
    return 0;
}
