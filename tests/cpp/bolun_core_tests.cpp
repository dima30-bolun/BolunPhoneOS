#include "bolun/bolun_kernel.h"

#include <cassert>
#include <cstring>
#include <iostream>

int main() {
    BolunKernel *kernel = nullptr;
    auto config = bolun_kernel_default_config();
    assert(bolun_kernel_create(&config, &kernel) == BOLUN_OK);
    assert(bolun_kernel_boot(kernel) == BOLUN_OK);

    BolunSystemStatus status{};
    assert(bolun_kernel_get_status(kernel, &status) == BOLUN_OK);
    assert(status.booted == 1);
    assert(std::strcmp(status.device_name, "Bolun Phone") == 0);
    assert(std::strcmp(status.company, "dima30") == 0);
    assert(status.app_count >= 10);

    BolunTileDescriptor tiles[BOLUN_MAX_TILES]{};
    std::size_t tile_count = 0;
    assert(bolun_kernel_list_tiles(kernel, tiles, BOLUN_MAX_TILES, &tile_count) == BOLUN_OK);
    assert(tile_count == status.tile_count);
    assert(std::strcmp(tiles[0].title, "Phone") == 0);

    char launch_message[128]{};
    assert(bolun_kernel_launch_app(kernel, "bolun-store", launch_message, sizeof(launch_message)) == BOLUN_OK);
    assert(std::strstr(launch_message, "Bolun Store") != nullptr);

    BolunMessageDescriptor message{};
    std::snprintf(message.sender, sizeof(message.sender), "Bolun");
    std::snprintf(message.recipient, sizeof(message.recipient), "Dima");
    std::snprintf(message.body, sizeof(message.body), "Privet from native core");
    assert(bolun_kernel_send_message(kernel, &message) == BOLUN_OK);

    BolunMessageDescriptor messages[BOLUN_MAX_MESSAGES]{};
    std::size_t message_count = 0;
    assert(bolun_kernel_list_messages(kernel, messages, BOLUN_MAX_MESSAGES, &message_count) == BOLUN_OK);
    assert(message_count == 1);
    assert(messages[0].delivered == 1);

    bolun_kernel_destroy(kernel);
    std::cout << "bolun_core_tests passed\n";
    return 0;
}
