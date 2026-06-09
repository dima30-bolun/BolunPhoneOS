#include "bolun/bolun_kernel.h"

#include <cassert>
#include <cstring>
#include <iostream>

static bool contains_feature(BolunFeatureDescriptor *features, std::size_t count, const char *area, const char *name) {
    for (std::size_t index = 0; index < count; ++index) {
        if (std::strcmp(features[index].area, area) == 0 && std::strcmp(features[index].name, name) == 0) {
            return true;
        }
    }
    return false;
}

static bool contains_tile(BolunTileDescriptor *tiles, std::size_t count, const char *app_id) {
    for (std::size_t index = 0; index < count; ++index) {
        if (std::strcmp(tiles[index].app_id, app_id) == 0) {
            return true;
        }
    }
    return false;
}

int main() {
    BolunKernel *kernel = nullptr;
    auto config = bolun_kernel_default_config();
    config.device_codename = "redmi-9c-nfc";
    config.developer_mode_enabled = 1;
    assert(bolun_kernel_create(&config, &kernel) == BOLUN_OK);
    assert(bolun_kernel_boot(kernel) == BOLUN_OK);

    BolunSystemStatus status{};
    assert(bolun_kernel_get_status(kernel, &status) == BOLUN_OK);
    assert(status.booted == 1);
    assert(std::strcmp(status.device_name, "Bolun Phone") == 0);
    assert(std::strcmp(status.company, "dima30") == 0);
    assert(status.app_count >= 80);
    assert(status.feature_count >= 170);
    assert(status.nfc_available == 1);
    assert(status.qr_code_available == 1);

    BolunDeviceProfile profile{};
    assert(bolun_kernel_get_device_profile(kernel, &profile) == BOLUN_OK);
    assert(std::strcmp(profile.codename, "redmi-9c-nfc") == 0);
    assert(profile.nfc_available == 1);
    assert(profile.qr_code_available == 1);

    BolunTileDescriptor tiles[BOLUN_MAX_TILES]{};
    std::size_t tile_count = 0;
    assert(bolun_kernel_list_tiles(kernel, tiles, BOLUN_MAX_TILES, &tile_count) == BOLUN_OK);
    assert(tile_count == status.tile_count);
    assert(std::strcmp(tiles[0].title, "Phone") == 0);
    assert(contains_tile(tiles, tile_count, "camera"));
    assert(contains_tile(tiles, tile_count, "calculator"));
    assert(contains_tile(tiles, tile_count, "developer-tools"));
    assert(contains_tile(tiles, tile_count, "qr-scanner"));

    BolunFeatureDescriptor features[BOLUN_MAX_FEATURES]{};
    std::size_t feature_count = 0;
    assert(bolun_kernel_list_features(kernel, features, BOLUN_MAX_FEATURES, &feature_count) == BOLUN_OK);
    assert(feature_count == status.feature_count);
    assert(contains_feature(features, feature_count, "Kernel", "Process scheduler"));
    assert(contains_feature(features, feature_count, "Dima30 Store", "Block unsafe removed apps"));
    assert(contains_feature(features, feature_count, "Developer Tools", "Python runtime"));
    assert(contains_feature(features, feature_count, "Extras", "Bolun Pay"));

    char launch_message[128]{};
    assert(bolun_kernel_launch_app(kernel, "dima30-store", launch_message, sizeof(launch_message)) == BOLUN_OK);
    assert(std::strstr(launch_message, "Dima30 Store") != nullptr);

    BolunStoreAppRemovalPolicy safe_removal{};
    assert(bolun_kernel_evaluate_store_removal(kernel, "demo.safe", "Developer unpublished this app.", 0, &safe_removal) == BOLUN_OK);
    assert(safe_removal.installed_copy_kept == 1);
    assert(safe_removal.launch_allowed == 1);
    assert(std::strstr(safe_removal.reason, "unpublished") != nullptr);

    BolunStoreAppRemovalPolicy unsafe_removal{};
    assert(bolun_kernel_evaluate_store_removal(kernel, "demo.unsafe", "Security malware signature match.", 1, &unsafe_removal) == BOLUN_OK);
    assert(unsafe_removal.installed_copy_kept == 1);
    assert(unsafe_removal.launch_allowed == 0);
    assert(unsafe_removal.security_removal == 1);

    bolun_kernel_destroy(kernel);

    config = bolun_kernel_default_config();
    config.device_codename = "redmi-9a";
    kernel = nullptr;
    assert(bolun_kernel_create(&config, &kernel) == BOLUN_OK);
    assert(bolun_kernel_get_device_profile(kernel, &profile) == BOLUN_OK);
    assert(profile.nfc_available == 0);
    assert(profile.qr_code_available == 1);
    bolun_kernel_destroy(kernel);

    std::cout << "bolun_core_tests passed\n";
    return 0;
}
