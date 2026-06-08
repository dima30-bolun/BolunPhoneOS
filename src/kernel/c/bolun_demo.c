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
