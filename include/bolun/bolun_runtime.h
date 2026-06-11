#ifndef BOLUN_RUNTIME_H
#define BOLUN_RUNTIME_H

/*
 * Bolun Phone 1.0 Beta runtime contracts.
 *
 * This header exposes a small, deterministic runtime that can be used by the
 * educational kernel, tests, and host tools.  The implementation keeps all
 * state in fixed-size arrays so it works without libc allocation after the
 * runtime object has been created.
 */

#include "bolun_result.h"
#include "bolun_types.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BOLUN_MAX_PROCESSES 64
#define BOLUN_MAX_MEMORY_BLOCKS 128
#define BOLUN_MAX_IPC_MESSAGES 128
#define BOLUN_MAX_TIMERS 64
#define BOLUN_MAX_INTERRUPTS 64
#define BOLUN_MAX_LOG_EVENTS 256
#define BOLUN_MAX_HAL_DEVICES 96
#define BOLUN_MAX_FILES 128
#define BOLUN_MAX_PACKAGES 64
#define BOLUN_MAX_LUMIA_PROFILES 16

typedef struct BolunRuntime BolunRuntime;

typedef enum BolunProcessState {
    BOLUN_PROCESS_CREATED = 1,
    BOLUN_PROCESS_READY = 2,
    BOLUN_PROCESS_RUNNING = 3,
    BOLUN_PROCESS_WAITING = 4,
    BOLUN_PROCESS_STOPPED = 5
} BolunProcessState;

typedef enum BolunHalDeviceClass {
    BOLUN_HAL_DISPLAY = 1,
    BOLUN_HAL_TOUCH = 2,
    BOLUN_HAL_BUTTONS = 3,
    BOLUN_HAL_HAPTICS = 4,
    BOLUN_HAL_AUDIO_OUT = 5,
    BOLUN_HAL_AUDIO_IN = 6,
    BOLUN_HAL_CAMERA = 7,
    BOLUN_HAL_FLASH = 8,
    BOLUN_HAL_USB = 9,
    BOLUN_HAL_WIFI = 10,
    BOLUN_HAL_BLUETOOTH = 11,
    BOLUN_HAL_GPS = 12,
    BOLUN_HAL_LTE = 13,
    BOLUN_HAL_SIM = 14,
    BOLUN_HAL_BATTERY = 15,
    BOLUN_HAL_CHARGING = 16,
    BOLUN_HAL_SD_CARD = 17,
    BOLUN_HAL_SENSOR = 18
} BolunHalDeviceClass;

typedef enum BolunPermissionBits {
    BOLUN_PERMISSION_NETWORK = 1u << 0,
    BOLUN_PERMISSION_CAMERA = 1u << 1,
    BOLUN_PERMISSION_MICROPHONE = 1u << 2,
    BOLUN_PERMISSION_LOCATION = 1u << 3,
    BOLUN_PERMISSION_STORAGE = 1u << 4,
    BOLUN_PERMISSION_PHONE = 1u << 5,
    BOLUN_PERMISSION_DEVELOPER = 1u << 6
} BolunPermissionBits;

typedef enum BolunSyscallNumber {
    BOLUN_SYSCALL_LOG = 1,
    BOLUN_SYSCALL_MEMORY_STATUS = 2,
    BOLUN_SYSCALL_HAL_STATUS = 3,
    BOLUN_SYSCALL_SECURITY_CHECK = 4
} BolunSyscallNumber;

typedef struct BolunRuntimeConfig {
    const char *device_codename;
    size_t total_memory_bytes;
    uint8_t developer_mode_enabled;
    uint8_t secure_boot_enabled;
} BolunRuntimeConfig;

typedef struct BolunProcessDescriptor {
    uint32_t pid;
    char app_id[BOLUN_TEXT_CAPACITY];
    char title[BOLUN_TEXT_CAPACITY];
    BolunProcessState state;
    uint32_t priority;
    uint32_t permissions;
    size_t memory_quota_bytes;
    size_t memory_used_bytes;
    uint64_t cpu_ticks;
    uint8_t sandboxed;
} BolunProcessDescriptor;

typedef struct BolunMemoryStatus {
    size_t total_bytes;
    size_t used_bytes;
    size_t free_bytes;
    size_t block_count;
} BolunMemoryStatus;

typedef struct BolunIpcMessage {
    uint32_t from_pid;
    uint32_t to_pid;
    char channel[BOLUN_TEXT_CAPACITY];
    char body[BOLUN_BODY_CAPACITY];
    uint64_t sequence;
} BolunIpcMessage;

typedef struct BolunTimerDescriptor {
    uint32_t timer_id;
    uint32_t owner_pid;
    uint64_t interval_ms;
    uint64_t remaining_ms;
    uint8_t repeating;
    uint8_t active;
} BolunTimerDescriptor;

typedef struct BolunInterruptDescriptor {
    uint32_t irq;
    char name[BOLUN_TEXT_CAPACITY];
    uint64_t handled_count;
    uint8_t enabled;
} BolunInterruptDescriptor;

typedef struct BolunLogEvent {
    uint64_t sequence;
    uint32_t severity;
    char area[BOLUN_TEXT_CAPACITY];
    char message[BOLUN_BODY_CAPACITY];
} BolunLogEvent;

typedef struct BolunHalDeviceDescriptor {
    BolunHalDeviceClass device_class;
    char device_id[BOLUN_TEXT_CAPACITY];
    char driver_name[BOLUN_TEXT_CAPACITY];
    char model[BOLUN_TEXT_CAPACITY];
    uint8_t present;
    uint8_t powered;
} BolunHalDeviceDescriptor;

typedef struct BolunFileRecord {
    char path[BOLUN_BODY_CAPACITY];
    char content_hash[BOLUN_TEXT_CAPACITY];
    size_t size_bytes;
    uint8_t archived;
    uint8_t deleted;
} BolunFileRecord;

typedef struct BolunPackageRecord {
    char package_id[BOLUN_TEXT_CAPACITY];
    char title[BOLUN_TEXT_CAPACITY];
    char version[BOLUN_TEXT_CAPACITY];
    char signature[BOLUN_TEXT_CAPACITY];
    uint8_t installed;
    uint8_t auto_update;
} BolunPackageRecord;

typedef struct BolunLumiaProfile {
    char codename[BOLUN_TEXT_CAPACITY];
    char display_name[BOLUN_TEXT_CAPACITY];
    uint8_t nfc_available;
    uint8_t lte_available;
    uint8_t glance_available;
} BolunLumiaProfile;

BolunRuntimeConfig bolun_runtime_default_config(void);
BolunResult bolun_runtime_create(const BolunRuntimeConfig *config, BolunRuntime **out_runtime);
void bolun_runtime_destroy(BolunRuntime *runtime);

BolunResult bolun_runtime_spawn_process(BolunRuntime *runtime, const char *app_id, const char *title, uint32_t permissions, size_t memory_quota_bytes, uint32_t *out_pid);
BolunResult bolun_runtime_schedule_next(BolunRuntime *runtime, BolunProcessDescriptor *out_process);
BolunResult bolun_runtime_get_process(const BolunRuntime *runtime, uint32_t pid, BolunProcessDescriptor *out_process);
BolunResult bolun_runtime_stop_process(BolunRuntime *runtime, uint32_t pid);

BolunResult bolun_runtime_allocate_memory(BolunRuntime *runtime, uint32_t pid, size_t bytes, uint32_t *out_block_id);
BolunResult bolun_runtime_free_memory(BolunRuntime *runtime, uint32_t pid, uint32_t block_id);
BolunResult bolun_runtime_memory_status(const BolunRuntime *runtime, BolunMemoryStatus *out_status);

BolunResult bolun_runtime_ipc_send(BolunRuntime *runtime, uint32_t from_pid, uint32_t to_pid, const char *channel, const char *body);
BolunResult bolun_runtime_ipc_receive(BolunRuntime *runtime, uint32_t to_pid, BolunIpcMessage *out_message);

BolunResult bolun_runtime_create_timer(BolunRuntime *runtime, uint32_t owner_pid, uint64_t interval_ms, uint8_t repeating, uint32_t *out_timer_id);
BolunResult bolun_runtime_tick(BolunRuntime *runtime, uint64_t elapsed_ms);

BolunResult bolun_runtime_register_interrupt(BolunRuntime *runtime, uint32_t irq, const char *name);
BolunResult bolun_runtime_raise_interrupt(BolunRuntime *runtime, uint32_t irq);

BolunResult bolun_runtime_log(BolunRuntime *runtime, uint32_t severity, const char *area, const char *message);
BolunResult bolun_runtime_list_logs(const BolunRuntime *runtime, BolunLogEvent *out_events, size_t capacity, size_t *out_count);
BolunResult bolun_runtime_syscall(BolunRuntime *runtime, uint32_t pid, BolunSyscallNumber number, const char *input, char *out_text, size_t out_text_size);

BolunResult bolun_runtime_hal_register(BolunRuntime *runtime, const BolunHalDeviceDescriptor *device);
BolunResult bolun_runtime_hal_list(const BolunRuntime *runtime, BolunHalDeviceDescriptor *out_devices, size_t capacity, size_t *out_count);
BolunResult bolun_runtime_hal_write(BolunRuntime *runtime, BolunHalDeviceClass device_class, const char *payload, char *out_response, size_t out_response_size);
BolunResult bolun_runtime_install_lumia_hal(BolunRuntime *runtime, const char *codename);
BolunResult bolun_runtime_list_lumia_profiles(BolunLumiaProfile *out_profiles, size_t capacity, size_t *out_count);

BolunResult bolun_runtime_add_file(BolunRuntime *runtime, const char *path, size_t size_bytes, const char *content_hash);
BolunResult bolun_runtime_search_files(const BolunRuntime *runtime, const char *query, BolunFileRecord *out_files, size_t capacity, size_t *out_count);
BolunResult bolun_runtime_move_file_to_trash(BolunRuntime *runtime, const char *path);
BolunResult bolun_runtime_archive_file(BolunRuntime *runtime, const char *path);
BolunResult bolun_runtime_check_file_integrity(const BolunRuntime *runtime, const char *path, const char *expected_hash);

BolunResult bolun_runtime_install_bapp(BolunRuntime *runtime, const char *ben_open_text, const char *signature, BolunPackageRecord *out_package);
BolunResult bolun_runtime_list_packages(const BolunRuntime *runtime, BolunPackageRecord *out_packages, size_t capacity, size_t *out_count);
BolunResult bolun_runtime_security_check(const BolunRuntime *runtime, uint32_t pid, uint32_t required_permission);

#ifdef __cplusplus
}
#endif

#endif
