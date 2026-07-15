#include "bolun/bolun_kernel.h"
#include "bolun/bolun_runtime.h"

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

    BolunRuntime *runtime = nullptr;
    BolunRuntimeConfig runtime_config = bolun_runtime_default_config();
    runtime_config.device_codename = "lumia-710";
    runtime_config.developer_mode_enabled = 1;
    assert(bolun_runtime_create(&runtime_config, &runtime) == BOLUN_OK);

    BolunLumiaProfile lumias[BOLUN_MAX_LUMIA_PROFILES]{};
    std::size_t lumia_count = 0;
    assert(bolun_runtime_list_lumia_profiles(lumias, BOLUN_MAX_LUMIA_PROFILES, &lumia_count) == BOLUN_OK);
    assert(lumia_count == 17);
    assert(std::strcmp(lumias[0].codename, "lumia-710") == 0);
    assert(std::strcmp(lumias[16].codename, "lumia-950-xl") == 0);

    BolunHalDeviceDescriptor devices[BOLUN_MAX_HAL_DEVICES]{};
    std::size_t device_count = 0;
    assert(bolun_runtime_hal_list(runtime, devices, BOLUN_MAX_HAL_DEVICES, &device_count) == BOLUN_OK);
    assert(device_count >= 18);
    char hal_response[128]{};
    assert(bolun_runtime_hal_write(runtime, BOLUN_HAL_CAMERA, "capture photo hdr", hal_response, sizeof(hal_response)) == BOLUN_OK);
    assert(std::strstr(hal_response, "bolun_lumia710_camera") != nullptr);

    uint32_t phone_pid = 0;
    uint32_t ai_pid = 0;
    assert(bolun_runtime_spawn_process(runtime, "phone", "Phone", BOLUN_PERMISSION_PHONE | BOLUN_PERMISSION_MICROPHONE, 1024 * 1024, &phone_pid) == BOLUN_OK);
    assert(bolun_runtime_spawn_process(runtime, "bolun-ai", "Bolun AI", BOLUN_PERMISSION_NETWORK | BOLUN_PERMISSION_MICROPHONE, 2 * 1024 * 1024, &ai_pid) == BOLUN_OK);
    BolunProcessDescriptor scheduled{};
    assert(bolun_runtime_schedule_next(runtime, &scheduled) == BOLUN_OK);
    assert(scheduled.pid == ai_pid || scheduled.pid == phone_pid);

    uint32_t block_id = 0;
    assert(bolun_runtime_allocate_memory(runtime, phone_pid, 4096, &block_id) == BOLUN_OK);
    BolunMemoryStatus memory{};
    assert(bolun_runtime_memory_status(runtime, &memory) == BOLUN_OK);
    assert(memory.used_bytes == 4096);
    assert(bolun_runtime_free_memory(runtime, phone_pid, block_id) == BOLUN_OK);

    assert(bolun_runtime_ipc_send(runtime, phone_pid, ai_pid, "assistant", "summarize call") == BOLUN_OK);
    BolunIpcMessage message{};
    assert(bolun_runtime_ipc_receive(runtime, ai_pid, &message) == BOLUN_OK);
    assert(std::strcmp(message.channel, "assistant") == 0);

    uint32_t timer_id = 0;
    assert(bolun_runtime_create_timer(runtime, ai_pid, 10, 0, &timer_id) == BOLUN_OK);
    assert(bolun_runtime_tick(runtime, 10) == BOLUN_OK);
    assert(bolun_runtime_ipc_receive(runtime, ai_pid, &message) == BOLUN_OK);
    assert(std::strstr(message.body, "timer:") != nullptr);

    assert(bolun_runtime_register_interrupt(runtime, 32, "display-vsync") == BOLUN_OK);
    assert(bolun_runtime_raise_interrupt(runtime, 32) == BOLUN_OK);
    char syscall_text[128]{};
    assert(bolun_runtime_syscall(runtime, phone_pid, BOLUN_SYSCALL_MEMORY_STATUS, "", syscall_text, sizeof(syscall_text)) == BOLUN_OK);
    assert(std::strstr(syscall_text, "memory:") != nullptr);
    assert(bolun_runtime_security_check(runtime, phone_pid, BOLUN_PERMISSION_CAMERA) == BOLUN_ERROR_INVALID_ARGUMENT);

    assert(bolun_runtime_add_file(runtime, "/Users/demo/Documents/readme.pdf", 2048, "sha256-demo") == BOLUN_OK);
    assert(bolun_runtime_check_file_integrity(runtime, "/Users/demo/Documents/readme.pdf", "sha256-demo") == BOLUN_OK);
    BolunFileRecord files[BOLUN_MAX_FILES]{};
    std::size_t file_count = 0;
    assert(bolun_runtime_search_files(runtime, "readme", files, BOLUN_MAX_FILES, &file_count) == BOLUN_OK);
    assert(file_count == 1);
    assert(bolun_runtime_archive_file(runtime, "/Users/demo/Documents/readme.pdf") == BOLUN_OK);
    assert(bolun_runtime_move_file_to_trash(runtime, "/Users/demo/Documents/readme.pdf") == BOLUN_OK);

    const char *ben_open = "id=demo.notes\ntitle=Demo Notes\nversion=1.0.0\nauto_update=true\n";
    BolunPackageRecord package{};
    assert(bolun_runtime_install_bapp(runtime, ben_open, "signed-by-dima30", &package) == BOLUN_OK);
    assert(package.installed == 1);
    assert(package.auto_update == 1);

    BolunLogEvent logs[BOLUN_MAX_LOG_EVENTS]{};
    std::size_t log_count = 0;
    assert(bolun_runtime_list_logs(runtime, logs, BOLUN_MAX_LOG_EVENTS, &log_count) == BOLUN_OK);
    assert(log_count >= 3);

    bolun_runtime_destroy(runtime);

    std::cout << "bolun_core_tests passed\n";
    return 0;
}
