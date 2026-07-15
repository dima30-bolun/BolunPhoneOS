#ifndef BOLUN_SYSTEM_H
#define BOLUN_SYSTEM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BOLUN10_MAX_PROCESSES 32
#define BOLUN10_MAX_BLOCKS 64
#define BOLUN10_MAX_FILES 64
#define BOLUN10_MAX_PACKAGES 32
#define BOLUN10_MAX_LOGS 128
#define BOLUN10_MAX_MESSAGES 64
#define BOLUN10_MAX_SOCKETS 24
#define BOLUN10_MAX_WINDOWS 24
#define BOLUN10_MAX_DRIVERS 32
#define BOLUN10_TEXT 64
#define BOLUN10_BODY 160

typedef enum Bolun10Cpu { BOLUN10_ARMV7 = 1, BOLUN10_ARM64 = 2, BOLUN10_X86 = 3, BOLUN10_X86_64 = 4 } Bolun10Cpu;
typedef enum Bolun10ProcessState { BOLUN10_PROC_EMPTY = 0, BOLUN10_PROC_READY = 1, BOLUN10_PROC_RUNNING = 2, BOLUN10_PROC_SLEEPING = 3, BOLUN10_PROC_STOPPED = 4 } Bolun10ProcessState;
typedef enum Bolun10Permission { BOLUN10_PERM_NET = 1u << 0, BOLUN10_PERM_CAMERA = 1u << 1, BOLUN10_PERM_AUDIO = 1u << 2, BOLUN10_PERM_STORAGE = 1u << 3, BOLUN10_PERM_PHONE = 1u << 4, BOLUN10_PERM_SYSTEM = 1u << 5 } Bolun10Permission;
typedef enum Bolun10DriverClass { BOLUN10_DRIVER_DISPLAY = 1, BOLUN10_DRIVER_AUDIO = 2, BOLUN10_DRIVER_CAMERA = 3, BOLUN10_DRIVER_NET = 4, BOLUN10_DRIVER_USB = 5, BOLUN10_DRIVER_POWER = 6, BOLUN10_DRIVER_STORAGE = 7 } Bolun10DriverClass;

typedef struct Bolun10BootImage { char target[BOLUN10_TEXT]; uint32_t version; uint32_t component_count; uint32_t image_crc; } Bolun10BootImage;
typedef struct Bolun10Process { uint32_t pid; char name[BOLUN10_TEXT]; Bolun10ProcessState state; uint32_t priority; uint32_t permissions; size_t memory_limit; size_t memory_used; uint64_t ticks; } Bolun10Process;
typedef struct Bolun10MemoryBlock { uint32_t id; uint32_t owner_pid; size_t bytes; uintptr_t virtual_address; uint8_t used; uint8_t writable; } Bolun10MemoryBlock;
typedef struct Bolun10File { char path[BOLUN10_TEXT]; uint32_t owner_pid; size_t bytes; uint32_t checksum; uint8_t directory; } Bolun10File;
typedef struct Bolun10Package { char id[BOLUN10_TEXT]; char title[BOLUN10_TEXT]; uint32_t version; uint32_t permissions; uint8_t installed; } Bolun10Package;
typedef struct Bolun10Log { uint32_t severity; char area[BOLUN10_TEXT]; char message[BOLUN10_BODY]; } Bolun10Log;
typedef struct Bolun10Message { uint32_t from_pid; uint32_t to_pid; char channel[BOLUN10_TEXT]; char body[BOLUN10_BODY]; uint64_t sequence; } Bolun10Message;
typedef struct Bolun10Socket { uint32_t id; uint32_t owner_pid; char local[BOLUN10_TEXT]; char remote[BOLUN10_TEXT]; uint32_t tx_packets; uint32_t rx_packets; uint8_t open; } Bolun10Socket;
typedef struct Bolun10Window { uint32_t id; uint32_t owner_pid; char title[BOLUN10_TEXT]; int x; int y; int width; int height; uint32_t color; uint8_t visible; } Bolun10Window;
typedef struct Bolun10Driver { Bolun10DriverClass type; char name[BOLUN10_TEXT]; uint32_t unit; uint8_t loaded; uint8_t powered; } Bolun10Driver;
typedef struct Bolun10System {
    Bolun10Cpu cpu;
    char device[BOLUN10_TEXT];
    Bolun10BootImage boot;
    Bolun10Process processes[BOLUN10_MAX_PROCESSES];
    Bolun10MemoryBlock memory[BOLUN10_MAX_BLOCKS];
    Bolun10File files[BOLUN10_MAX_FILES];
    Bolun10Package packages[BOLUN10_MAX_PACKAGES];
    Bolun10Log logs[BOLUN10_MAX_LOGS];
    Bolun10Message messages[BOLUN10_MAX_MESSAGES];
    Bolun10Socket sockets[BOLUN10_MAX_SOCKETS];
    Bolun10Window windows[BOLUN10_MAX_WINDOWS];
    Bolun10Driver drivers[BOLUN10_MAX_DRIVERS];
    size_t process_count;
    size_t memory_count;
    size_t file_count;
    size_t package_count;
    size_t log_count;
    size_t message_count;
    size_t socket_count;
    size_t window_count;
    size_t driver_count;
    size_t scheduler_cursor;
    uint32_t next_pid;
    uint32_t next_block;
    uint32_t next_socket;
    uint32_t next_window;
    uint64_t next_message;
    size_t total_memory;
    uint8_t low_power_mode;
    uint32_t update_generation;
} Bolun10System;

int bolun10_system_init(Bolun10System *system, Bolun10Cpu cpu, const char *device, size_t total_memory);
int bolun10_boot_prepare(Bolun10System *system, const char *target, uint32_t component_count);
uint32_t bolun10_crc32_text(const char *text);
int bolun10_log(Bolun10System *system, uint32_t severity, const char *area, const char *message);
int bolun10_process_spawn(Bolun10System *system, const char *name, uint32_t permissions, size_t memory_limit, uint32_t *out_pid);
int bolun10_process_schedule(Bolun10System *system, Bolun10Process *out_process);
int bolun10_memory_alloc(Bolun10System *system, uint32_t pid, size_t bytes, uint32_t *out_block);
int bolun10_memory_map(Bolun10System *system, uint32_t block_id, uintptr_t virtual_address, uint8_t writable);
int bolun10_memory_free(Bolun10System *system, uint32_t block_id);
int bolun10_bfs_create(Bolun10System *system, const char *path, uint32_t owner_pid, size_t bytes, const char *contents);
int bolun10_bfs_read(const Bolun10System *system, const char *path, Bolun10File *out_file);
int bolun10_bapp_install(Bolun10System *system, const char *manifest, Bolun10Package *out_package);
int bolun10_security_check(const Bolun10System *system, uint32_t pid, uint32_t permission);
int bolun10_ipc_send(Bolun10System *system, uint32_t from_pid, uint32_t to_pid, const char *channel, const char *body);
int bolun10_ipc_receive(Bolun10System *system, uint32_t to_pid, Bolun10Message *out_message);
int bolun10_driver_register(Bolun10System *system, Bolun10DriverClass type, const char *name, uint32_t unit);
int bolun10_driver_power(Bolun10System *system, const char *name, uint8_t powered);
int bolun10_socket_open(Bolun10System *system, uint32_t owner_pid, const char *local, const char *remote, uint32_t *out_socket);
int bolun10_socket_send(Bolun10System *system, uint32_t socket_id, const char *payload);
int bolun10_window_create(Bolun10System *system, uint32_t owner_pid, const char *title, int width, int height, uint32_t *out_window);
int bolun10_compositor_frame(const Bolun10System *system, char *out_text, size_t out_size);
int bolun10_power_set_low_power(Bolun10System *system, uint8_t enabled);
int bolun10_update_apply(Bolun10System *system, const char *version, uint32_t signature);
int bolun10_recovery_plan(Bolun10System *system, const char *reason);
int bolun10_diagnostics_report(const Bolun10System *system, char *out_text, size_t out_size);
int bolun10_ui_render_home(const Bolun10System *system, char *out_text, size_t out_size);

#ifdef __cplusplus
}
#endif

#endif
