#include "bolun_system.h"
#include "bolun_hal.h"

#include <assert.h>
#include <string.h>

static int ok(void *ctx) { (void)ctx; return 0; }
static int io(void *ctx, uint32_t request, void *data) { (void)ctx; (void)request; (void)data; return 0; }

int main(void) {
    Bolun10System system;
    assert(bolun10_system_init(&system, BOLUN10_ARMV7, "lumia710", 512u * 1024u * 1024u) == 0);
    assert(bolun10_boot_prepare(&system, "lumia710", 8) == 0);
    uint32_t shell_pid = 0;
    assert(bolun10_process_spawn(&system, "bolun-shell", BOLUN10_PERM_SYSTEM | BOLUN10_PERM_STORAGE | BOLUN10_PERM_NET, 8u * 1024u * 1024u, &shell_pid) == 0);
    uint32_t camera_pid = 0;
    assert(bolun10_process_spawn(&system, "camera", BOLUN10_PERM_CAMERA | BOLUN10_PERM_STORAGE, 4u * 1024u * 1024u, &camera_pid) == 0);
    Bolun10Process scheduled;
    assert(bolun10_process_schedule(&system, &scheduled) == 0);
    uint32_t block = 0;
    assert(bolun10_memory_alloc(&system, shell_pid, 4096, &block) == 0);
    assert(bolun10_memory_map(&system, block, 0x100000u, 1) == 0);
    assert(bolun10_bfs_create(&system, "/home/shell/theme.bfs", shell_pid, 128, "theme=dark") == 0);
    Bolun10File file;
    assert(bolun10_bfs_read(&system, "/home/shell/theme.bfs", &file) == 0);
    const char *manifest = "id=bolun.camera\ntitle=Camera\nversion=1\npermission=camera\n";
    Bolun10Package package;
    assert(bolun10_bapp_install(&system, manifest, &package) == 0);
    assert(strcmp(package.id, "bolun.camera") == 0);
    assert(bolun10_security_check(&system, camera_pid, BOLUN10_PERM_CAMERA) == 0);
    assert(bolun10_security_check(&system, shell_pid, BOLUN10_PERM_CAMERA) != 0);
    assert(bolun10_ipc_send(&system, shell_pid, camera_pid, "capture", "prepare") == 0);
    Bolun10Message message;
    assert(bolun10_ipc_receive(&system, camera_pid, &message) == 0);
    assert(strcmp(message.channel, "capture") == 0);
    assert(bolun10_driver_register(&system, BOLUN10_DRIVER_DISPLAY, "panel0", 0) == 0);
    assert(bolun10_driver_power(&system, "panel0", 0) == 0);
    uint32_t socket = 0;
    assert(bolun10_socket_open(&system, shell_pid, "10.0.0.2:2000", "10.0.0.1:80", &socket) == 0);
    assert(bolun10_socket_send(&system, socket, "ping") == 0);
    uint32_t window = 0;
    assert(bolun10_window_create(&system, shell_pid, "Home", 480, 800, &window) == 0);
    char frame[160];
    assert(bolun10_compositor_frame(&system, frame, sizeof(frame)) == 0);
    assert(strstr(frame, "windows=1") != 0);
    assert(bolun10_power_set_low_power(&system, 1) == 0);
    const char *version = "1";
    assert(bolun10_update_apply(&system, version, bolun10_crc32_text(version) ^ 0xB01010u) == 0);
    assert(bolun10_recovery_plan(&system, "post-update checkpoint") == 0);
    char diagnostics[256];
    assert(bolun10_diagnostics_report(&system, diagnostics, sizeof(diagnostics)) == 0);
    assert(strstr(diagnostics, "updates=1") != 0);
    BolunHalDevice hal = { BOLUN_HAL_DISPLAY, "panel0", 0, 0, ok, ok, ok, io };
    assert(bolun_hal_validate(&hal) == 0);
    char home[160];
    assert(bolun10_ui_render_home(&system, home, sizeof(home)) == 0);
    assert(strstr(home, "Bolun UI") != 0);
    assert(bolun10_memory_free(&system, block) == 0);
    return 0;
}
