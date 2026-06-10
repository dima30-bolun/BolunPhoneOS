#include "bolun/bolun_kernel.h"

#include <stdio.h>

int main(void) {
    BolunKernel *kernel = NULL;
    BolunKernelConfig config = bolun_kernel_default_config();
    BolunResult result = bolun_kernel_create(&config, &kernel);
    if (result != BOLUN_OK) {
        fprintf(stderr, "create failed: %s\n", bolun_result_message(result));
        return 1;
    }

    bolun_kernel_boot(kernel);
    BolunSystemStatus status;
    bolun_kernel_get_status(kernel, &status);
    printf("Bolun Phone 1.0: %zu subsystems, %zu features, NFC=%s, QR=%s\n", status.subsystem_count, status.feature_count, status.nfc_available ? "yes" : "no", status.qr_code_available ? "yes" : "no");

    BolunSubsystemDescriptor subsystems[BOLUN_MAX_SUBSYSTEMS];
    size_t subsystem_count = 0;
    bolun_kernel_list_subsystems(kernel, subsystems, BOLUN_MAX_SUBSYSTEMS, &subsystem_count);
    printf("Bolun Phone subsystems:\n");
    for (size_t index = 0; index < subsystem_count; ++index) {
        printf("- %s: %s\n", subsystems[index].title, subsystems[index].provided_services);
    }

    BolunTileDescriptor tiles[BOLUN_MAX_TILES];
    size_t tile_count = 0;
    bolun_kernel_list_tiles(kernel, tiles, BOLUN_MAX_TILES, &tile_count);
    printf("Bolun Phone home:\n");
    for (size_t index = 0; index < tile_count; ++index) {
        printf("[%s] %s\n", tiles[index].glyph, tiles[index].title);
    }

    bolun_kernel_destroy(kernel);
    return 0;
}
