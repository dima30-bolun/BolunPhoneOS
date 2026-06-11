#include "bolun/bolun_runtime.h"

/*
 * Fixed-capacity Bolun Phone runtime implementation.
 *
 * The runtime models the operating-system services requested for Bolun Phone
 * 1.0 Beta while remaining portable enough to execute inside the repository's
 * host-side tests.  Each subsystem is intentionally deterministic: no threads,
 * no background allocation, and no device access outside the Hardware
 * Abstraction Layer registry.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOLUN_DEFAULT_MEMORY_BYTES (128u * 1024u * 1024u)

typedef struct BolunMemoryBlock {
    uint32_t block_id;
    uint32_t owner_pid;
    size_t bytes;
    uint8_t used;
} BolunMemoryBlock;

struct BolunRuntime {
    BolunRuntimeConfig config;
    char device_codename[BOLUN_TEXT_CAPACITY];
    size_t total_memory_bytes;
    uint32_t next_pid;
    uint32_t next_block_id;
    uint32_t next_timer_id;
    uint64_t next_ipc_sequence;
    uint64_t next_log_sequence;
    size_t scheduler_cursor;
    BolunProcessDescriptor processes[BOLUN_MAX_PROCESSES];
    size_t process_count;
    BolunMemoryBlock memory_blocks[BOLUN_MAX_MEMORY_BLOCKS];
    size_t memory_block_count;
    BolunIpcMessage ipc_messages[BOLUN_MAX_IPC_MESSAGES];
    size_t ipc_count;
    BolunTimerDescriptor timers[BOLUN_MAX_TIMERS];
    size_t timer_count;
    BolunInterruptDescriptor interrupts[BOLUN_MAX_INTERRUPTS];
    size_t interrupt_count;
    BolunLogEvent logs[BOLUN_MAX_LOG_EVENTS];
    size_t log_count;
    BolunHalDeviceDescriptor hal_devices[BOLUN_MAX_HAL_DEVICES];
    size_t hal_device_count;
    BolunFileRecord files[BOLUN_MAX_FILES];
    size_t file_count;
    BolunPackageRecord packages[BOLUN_MAX_PACKAGES];
    size_t package_count;
};

static void copy_text(char *destination, size_t capacity, const char *source) {
    if (capacity == 0) {
        return;
    }
    snprintf(destination, capacity, "%s", source == NULL ? "" : source);
}

static int text_contains(const char *text, const char *query) {
    if (text == NULL || query == NULL || query[0] == '\0') {
        return 0;
    }
    return strstr(text, query) != NULL;
}

static size_t used_memory_bytes(const BolunRuntime *runtime) {
    size_t used = 0;
    for (size_t index = 0; index < runtime->memory_block_count; ++index) {
        if (runtime->memory_blocks[index].used) {
            used += runtime->memory_blocks[index].bytes;
        }
    }
    return used;
}

static BolunProcessDescriptor *find_process(BolunRuntime *runtime, uint32_t pid) {
    for (size_t index = 0; index < runtime->process_count; ++index) {
        if (runtime->processes[index].pid == pid && runtime->processes[index].state != BOLUN_PROCESS_STOPPED) {
            return &runtime->processes[index];
        }
    }
    return NULL;
}

static const BolunProcessDescriptor *find_process_const(const BolunRuntime *runtime, uint32_t pid) {
    for (size_t index = 0; index < runtime->process_count; ++index) {
        if (runtime->processes[index].pid == pid && runtime->processes[index].state != BOLUN_PROCESS_STOPPED) {
            return &runtime->processes[index];
        }
    }
    return NULL;
}

static BolunResult append_log(BolunRuntime *runtime, uint32_t severity, const char *area, const char *message) {
    if (runtime == NULL || area == NULL || area[0] == '\0' || message == NULL || message[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (runtime->log_count == BOLUN_MAX_LOG_EVENTS) {
        memmove(runtime->logs, runtime->logs + 1, (BOLUN_MAX_LOG_EVENTS - 1) * sizeof(BolunLogEvent));
        runtime->log_count = BOLUN_MAX_LOG_EVENTS - 1;
    }
    BolunLogEvent *event = &runtime->logs[runtime->log_count++];
    memset(event, 0, sizeof(*event));
    event->sequence = runtime->next_log_sequence++;
    event->severity = severity;
    copy_text(event->area, sizeof(event->area), area);
    copy_text(event->message, sizeof(event->message), message);
    return BOLUN_OK;
}

static BolunResult register_device(BolunRuntime *runtime, BolunHalDeviceClass device_class, const char *id, const char *driver, const char *model, uint8_t present) {
    BolunHalDeviceDescriptor device;
    memset(&device, 0, sizeof(device));
    device.device_class = device_class;
    copy_text(device.device_id, sizeof(device.device_id), id);
    copy_text(device.driver_name, sizeof(device.driver_name), driver);
    copy_text(device.model, sizeof(device.model), model);
    device.present = present ? 1 : 0;
    device.powered = present ? 1 : 0;
    return bolun_runtime_hal_register(runtime, &device);
}

static const BolunLumiaProfile lumia_profiles[] = {
    {"lumia-625", "Lumia 625", 0, 1, 0},
    {"lumia-520", "Lumia 520", 0, 0, 0},
    {"lumia-525", "Lumia 525", 0, 0, 0},
    {"lumia-530", "Lumia 530", 0, 0, 0},
    {"lumia-535", "Lumia 535", 0, 0, 0},
    {"lumia-630", "Lumia 630", 0, 0, 0},
    {"lumia-635", "Lumia 635", 0, 1, 0},
    {"lumia-640", "Lumia 640", 1, 1, 1},
    {"lumia-730", "Lumia 730", 1, 1, 1},
    {"lumia-735", "Lumia 735", 1, 1, 1},
    {"lumia-830", "Lumia 830", 1, 1, 1},
    {"lumia-920", "Lumia 920", 1, 1, 1},
    {"lumia-925", "Lumia 925", 1, 1, 1},
    {"lumia-930", "Lumia 930", 1, 1, 1},
    {"lumia-950", "Lumia 950", 1, 1, 1},
    {"lumia-950-xl", "Lumia 950 XL", 1, 1, 1},
};

BolunRuntimeConfig bolun_runtime_default_config(void) {
    BolunRuntimeConfig config;
    config.device_codename = "lumia-950";
    config.total_memory_bytes = BOLUN_DEFAULT_MEMORY_BYTES;
    config.developer_mode_enabled = 0;
    config.secure_boot_enabled = 1;
    return config;
}

BolunResult bolun_runtime_create(const BolunRuntimeConfig *config, BolunRuntime **out_runtime) {
    if (out_runtime == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    BolunRuntimeConfig effective = config == NULL ? bolun_runtime_default_config() : *config;
    BolunRuntime *runtime = (BolunRuntime *)calloc(1, sizeof(BolunRuntime));
    if (runtime == NULL) {
        return BOLUN_ERROR_INTERNAL;
    }
    runtime->config = effective;
    runtime->total_memory_bytes = effective.total_memory_bytes == 0 ? BOLUN_DEFAULT_MEMORY_BYTES : effective.total_memory_bytes;
    runtime->next_pid = 100;
    runtime->next_block_id = 1;
    runtime->next_timer_id = 1;
    runtime->next_ipc_sequence = 1;
    runtime->next_log_sequence = 1;
    copy_text(runtime->device_codename, sizeof(runtime->device_codename), effective.device_codename == NULL ? "lumia-950" : effective.device_codename);
    append_log(runtime, 1, "Runtime", "Bolun runtime created");
    bolun_runtime_install_lumia_hal(runtime, runtime->device_codename);
    *out_runtime = runtime;
    return BOLUN_OK;
}

void bolun_runtime_destroy(BolunRuntime *runtime) {
    free(runtime);
}

BolunResult bolun_runtime_spawn_process(BolunRuntime *runtime, const char *app_id, const char *title, uint32_t permissions, size_t memory_quota_bytes, uint32_t *out_pid) {
    if (runtime == NULL || app_id == NULL || app_id[0] == '\0' || title == NULL || title[0] == '\0' || out_pid == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (runtime->process_count >= BOLUN_MAX_PROCESSES) {
        return BOLUN_ERROR_FULL;
    }
    BolunProcessDescriptor *process = &runtime->processes[runtime->process_count++];
    memset(process, 0, sizeof(*process));
    process->pid = runtime->next_pid++;
    copy_text(process->app_id, sizeof(process->app_id), app_id);
    copy_text(process->title, sizeof(process->title), title);
    process->state = BOLUN_PROCESS_READY;
    process->priority = permissions & BOLUN_PERMISSION_DEVELOPER ? 2 : 1;
    process->permissions = permissions;
    process->memory_quota_bytes = memory_quota_bytes == 0 ? (4u * 1024u * 1024u) : memory_quota_bytes;
    process->sandboxed = (permissions & BOLUN_PERMISSION_DEVELOPER) == 0 ? 1 : 0;
    *out_pid = process->pid;
    append_log(runtime, 1, "Process", "Process spawned");
    return BOLUN_OK;
}

BolunResult bolun_runtime_schedule_next(BolunRuntime *runtime, BolunProcessDescriptor *out_process) {
    if (runtime == NULL || out_process == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (runtime->process_count == 0) {
        return BOLUN_ERROR_NOT_FOUND;
    }
    for (size_t attempt = 0; attempt < runtime->process_count; ++attempt) {
        runtime->scheduler_cursor = (runtime->scheduler_cursor + 1) % runtime->process_count;
        BolunProcessDescriptor *candidate = &runtime->processes[runtime->scheduler_cursor];
        if (candidate->state == BOLUN_PROCESS_READY || candidate->state == BOLUN_PROCESS_RUNNING) {
            for (size_t index = 0; index < runtime->process_count; ++index) {
                if (runtime->processes[index].state == BOLUN_PROCESS_RUNNING) {
                    runtime->processes[index].state = BOLUN_PROCESS_READY;
                }
            }
            candidate->state = BOLUN_PROCESS_RUNNING;
            candidate->cpu_ticks += 1;
            *out_process = *candidate;
            return BOLUN_OK;
        }
    }
    return BOLUN_ERROR_NOT_FOUND;
}

BolunResult bolun_runtime_get_process(const BolunRuntime *runtime, uint32_t pid, BolunProcessDescriptor *out_process) {
    if (runtime == NULL || out_process == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    const BolunProcessDescriptor *process = find_process_const(runtime, pid);
    if (process == NULL) {
        return BOLUN_ERROR_NOT_FOUND;
    }
    *out_process = *process;
    return BOLUN_OK;
}

BolunResult bolun_runtime_stop_process(BolunRuntime *runtime, uint32_t pid) {
    if (runtime == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    BolunProcessDescriptor *process = find_process(runtime, pid);
    if (process == NULL) {
        return BOLUN_ERROR_NOT_FOUND;
    }
    for (size_t index = 0; index < runtime->memory_block_count; ++index) {
        if (runtime->memory_blocks[index].owner_pid == pid && runtime->memory_blocks[index].used) {
            runtime->memory_blocks[index].used = 0;
        }
    }
    process->memory_used_bytes = 0;
    process->state = BOLUN_PROCESS_STOPPED;
    append_log(runtime, 2, "Process", "Process stopped");
    return BOLUN_OK;
}

BolunResult bolun_runtime_allocate_memory(BolunRuntime *runtime, uint32_t pid, size_t bytes, uint32_t *out_block_id) {
    if (runtime == NULL || bytes == 0 || out_block_id == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    BolunProcessDescriptor *process = find_process(runtime, pid);
    if (process == NULL) {
        return BOLUN_ERROR_NOT_FOUND;
    }
    if (runtime->memory_block_count >= BOLUN_MAX_MEMORY_BLOCKS || used_memory_bytes(runtime) + bytes > runtime->total_memory_bytes || process->memory_used_bytes + bytes > process->memory_quota_bytes) {
        return BOLUN_ERROR_FULL;
    }
    BolunMemoryBlock *block = &runtime->memory_blocks[runtime->memory_block_count++];
    block->block_id = runtime->next_block_id++;
    block->owner_pid = pid;
    block->bytes = bytes;
    block->used = 1;
    process->memory_used_bytes += bytes;
    *out_block_id = block->block_id;
    return BOLUN_OK;
}

BolunResult bolun_runtime_free_memory(BolunRuntime *runtime, uint32_t pid, uint32_t block_id) {
    if (runtime == NULL || block_id == 0) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    BolunProcessDescriptor *process = find_process(runtime, pid);
    if (process == NULL) {
        return BOLUN_ERROR_NOT_FOUND;
    }
    for (size_t index = 0; index < runtime->memory_block_count; ++index) {
        BolunMemoryBlock *block = &runtime->memory_blocks[index];
        if (block->block_id == block_id && block->owner_pid == pid && block->used) {
            block->used = 0;
            process->memory_used_bytes = process->memory_used_bytes >= block->bytes ? process->memory_used_bytes - block->bytes : 0;
            return BOLUN_OK;
        }
    }
    return BOLUN_ERROR_NOT_FOUND;
}

BolunResult bolun_runtime_memory_status(const BolunRuntime *runtime, BolunMemoryStatus *out_status) {
    if (runtime == NULL || out_status == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    out_status->total_bytes = runtime->total_memory_bytes;
    out_status->used_bytes = used_memory_bytes(runtime);
    out_status->free_bytes = out_status->total_bytes >= out_status->used_bytes ? out_status->total_bytes - out_status->used_bytes : 0;
    out_status->block_count = runtime->memory_block_count;
    return BOLUN_OK;
}

BolunResult bolun_runtime_ipc_send(BolunRuntime *runtime, uint32_t from_pid, uint32_t to_pid, const char *channel, const char *body) {
    if (runtime == NULL || channel == NULL || channel[0] == '\0' || body == NULL || body[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if ((from_pid != 0 && find_process(runtime, from_pid) == NULL) || find_process(runtime, to_pid) == NULL) {
        return BOLUN_ERROR_NOT_FOUND;
    }
    if (runtime->ipc_count >= BOLUN_MAX_IPC_MESSAGES) {
        return BOLUN_ERROR_FULL;
    }
    BolunIpcMessage *message = &runtime->ipc_messages[runtime->ipc_count++];
    memset(message, 0, sizeof(*message));
    message->from_pid = from_pid;
    message->to_pid = to_pid;
    message->sequence = runtime->next_ipc_sequence++;
    copy_text(message->channel, sizeof(message->channel), channel);
    copy_text(message->body, sizeof(message->body), body);
    return BOLUN_OK;
}

BolunResult bolun_runtime_ipc_receive(BolunRuntime *runtime, uint32_t to_pid, BolunIpcMessage *out_message) {
    if (runtime == NULL || out_message == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (find_process(runtime, to_pid) == NULL) {
        return BOLUN_ERROR_NOT_FOUND;
    }
    for (size_t index = 0; index < runtime->ipc_count; ++index) {
        if (runtime->ipc_messages[index].to_pid == to_pid) {
            *out_message = runtime->ipc_messages[index];
            memmove(&runtime->ipc_messages[index], &runtime->ipc_messages[index + 1], (runtime->ipc_count - index - 1) * sizeof(BolunIpcMessage));
            runtime->ipc_count -= 1;
            return BOLUN_OK;
        }
    }
    return BOLUN_ERROR_NOT_FOUND;
}

BolunResult bolun_runtime_create_timer(BolunRuntime *runtime, uint32_t owner_pid, uint64_t interval_ms, uint8_t repeating, uint32_t *out_timer_id) {
    if (runtime == NULL || interval_ms == 0 || out_timer_id == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (find_process(runtime, owner_pid) == NULL) {
        return BOLUN_ERROR_NOT_FOUND;
    }
    if (runtime->timer_count >= BOLUN_MAX_TIMERS) {
        return BOLUN_ERROR_FULL;
    }
    BolunTimerDescriptor *timer = &runtime->timers[runtime->timer_count++];
    memset(timer, 0, sizeof(*timer));
    timer->timer_id = runtime->next_timer_id++;
    timer->owner_pid = owner_pid;
    timer->interval_ms = interval_ms;
    timer->remaining_ms = interval_ms;
    timer->repeating = repeating ? 1 : 0;
    timer->active = 1;
    *out_timer_id = timer->timer_id;
    return BOLUN_OK;
}

BolunResult bolun_runtime_tick(BolunRuntime *runtime, uint64_t elapsed_ms) {
    if (runtime == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < runtime->timer_count; ++index) {
        BolunTimerDescriptor *timer = &runtime->timers[index];
        if (!timer->active) {
            continue;
        }
        if (elapsed_ms >= timer->remaining_ms) {
            char body[BOLUN_BODY_CAPACITY];
            snprintf(body, sizeof(body), "timer:%u", timer->timer_id);
            bolun_runtime_ipc_send(runtime, 0, timer->owner_pid, "timer", body);
            if (timer->repeating) {
                timer->remaining_ms = timer->interval_ms;
            } else {
                timer->active = 0;
            }
        } else {
            timer->remaining_ms -= elapsed_ms;
        }
    }
    return BOLUN_OK;
}

BolunResult bolun_runtime_register_interrupt(BolunRuntime *runtime, uint32_t irq, const char *name) {
    if (runtime == NULL || name == NULL || name[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < runtime->interrupt_count; ++index) {
        if (runtime->interrupts[index].irq == irq) {
            copy_text(runtime->interrupts[index].name, sizeof(runtime->interrupts[index].name), name);
            runtime->interrupts[index].enabled = 1;
            return BOLUN_OK;
        }
    }
    if (runtime->interrupt_count >= BOLUN_MAX_INTERRUPTS) {
        return BOLUN_ERROR_FULL;
    }
    BolunInterruptDescriptor *descriptor = &runtime->interrupts[runtime->interrupt_count++];
    memset(descriptor, 0, sizeof(*descriptor));
    descriptor->irq = irq;
    descriptor->enabled = 1;
    copy_text(descriptor->name, sizeof(descriptor->name), name);
    return BOLUN_OK;
}

BolunResult bolun_runtime_raise_interrupt(BolunRuntime *runtime, uint32_t irq) {
    if (runtime == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < runtime->interrupt_count; ++index) {
        if (runtime->interrupts[index].irq == irq && runtime->interrupts[index].enabled) {
            runtime->interrupts[index].handled_count += 1;
            append_log(runtime, 1, "Interrupt", runtime->interrupts[index].name);
            return BOLUN_OK;
        }
    }
    return BOLUN_ERROR_NOT_FOUND;
}

BolunResult bolun_runtime_log(BolunRuntime *runtime, uint32_t severity, const char *area, const char *message) {
    return append_log(runtime, severity, area, message);
}

BolunResult bolun_runtime_list_logs(const BolunRuntime *runtime, BolunLogEvent *out_events, size_t capacity, size_t *out_count) {
    if (runtime == NULL || out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    *out_count = runtime->log_count;
    if (out_events != NULL) {
        size_t count = runtime->log_count < capacity ? runtime->log_count : capacity;
        memcpy(out_events, runtime->logs, count * sizeof(BolunLogEvent));
    }
    return BOLUN_OK;
}

BolunResult bolun_runtime_syscall(BolunRuntime *runtime, uint32_t pid, BolunSyscallNumber number, const char *input, char *out_text, size_t out_text_size) {
    if (runtime == NULL || out_text == NULL || out_text_size == 0) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (pid != 0 && find_process(runtime, pid) == NULL) {
        return BOLUN_ERROR_NOT_FOUND;
    }
    if (number == BOLUN_SYSCALL_LOG) {
        BolunResult result = append_log(runtime, 1, "Syscall", input == NULL ? "empty" : input);
        snprintf(out_text, out_text_size, "log:%s", bolun_result_message(result));
        return result;
    }
    if (number == BOLUN_SYSCALL_MEMORY_STATUS) {
        BolunMemoryStatus status;
        BolunResult result = bolun_runtime_memory_status(runtime, &status);
        snprintf(out_text, out_text_size, "memory:%zu/%zu", status.used_bytes, status.total_bytes);
        return result;
    }
    if (number == BOLUN_SYSCALL_HAL_STATUS) {
        snprintf(out_text, out_text_size, "hal:%zu", runtime->hal_device_count);
        return BOLUN_OK;
    }
    if (number == BOLUN_SYSCALL_SECURITY_CHECK) {
        uint32_t permission = input == NULL ? 0 : (uint32_t)strtoul(input, NULL, 10);
        BolunResult result = bolun_runtime_security_check(runtime, pid, permission);
        snprintf(out_text, out_text_size, "security:%s", bolun_result_message(result));
        return result;
    }
    return BOLUN_ERROR_NOT_FOUND;
}

BolunResult bolun_runtime_hal_register(BolunRuntime *runtime, const BolunHalDeviceDescriptor *device) {
    if (runtime == NULL || device == NULL || device->device_id[0] == '\0' || device->driver_name[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < runtime->hal_device_count; ++index) {
        if (runtime->hal_devices[index].device_class == device->device_class && strcmp(runtime->hal_devices[index].device_id, device->device_id) == 0) {
            runtime->hal_devices[index] = *device;
            return BOLUN_OK;
        }
    }
    if (runtime->hal_device_count >= BOLUN_MAX_HAL_DEVICES) {
        return BOLUN_ERROR_FULL;
    }
    runtime->hal_devices[runtime->hal_device_count++] = *device;
    return BOLUN_OK;
}

BolunResult bolun_runtime_hal_list(const BolunRuntime *runtime, BolunHalDeviceDescriptor *out_devices, size_t capacity, size_t *out_count) {
    if (runtime == NULL || out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    *out_count = runtime->hal_device_count;
    if (out_devices != NULL) {
        size_t count = runtime->hal_device_count < capacity ? runtime->hal_device_count : capacity;
        memcpy(out_devices, runtime->hal_devices, count * sizeof(BolunHalDeviceDescriptor));
    }
    return BOLUN_OK;
}

BolunResult bolun_runtime_hal_write(BolunRuntime *runtime, BolunHalDeviceClass device_class, const char *payload, char *out_response, size_t out_response_size) {
    if (runtime == NULL || payload == NULL || payload[0] == '\0' || out_response == NULL || out_response_size == 0) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < runtime->hal_device_count; ++index) {
        BolunHalDeviceDescriptor *device = &runtime->hal_devices[index];
        if (device->device_class == device_class && device->present && device->powered) {
            snprintf(out_response, out_response_size, "%s accepted %s", device->driver_name, payload);
            return BOLUN_OK;
        }
    }
    return BOLUN_ERROR_NOT_FOUND;
}

BolunResult bolun_runtime_install_lumia_hal(BolunRuntime *runtime, const char *codename) {
    if (runtime == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    const BolunLumiaProfile *profile = &lumia_profiles[14];
    for (size_t index = 0; index < sizeof(lumia_profiles) / sizeof(lumia_profiles[0]); ++index) {
        if (codename != NULL && strcmp(lumia_profiles[index].codename, codename) == 0) {
            profile = &lumia_profiles[index];
            break;
        }
    }
    register_device(runtime, BOLUN_HAL_DISPLAY, "display0", "bolun_lumia_display", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_TOUCH, "touch0", "bolun_lumia_touch", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_BUTTONS, "buttons0", "bolun_lumia_buttons", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_HAPTICS, "vibrator0", "bolun_lumia_haptics", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_AUDIO_OUT, "speaker0", "bolun_lumia_speaker", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_AUDIO_IN, "mic0", "bolun_lumia_microphone", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_CAMERA, "camera0", "bolun_lumia_camera", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_FLASH, "flash0", "bolun_lumia_flash", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_USB, "usb0", "bolun_lumia_usb", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_WIFI, "wifi0", "bolun_lumia_wifi", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_BLUETOOTH, "bt0", "bolun_lumia_bluetooth", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_GPS, "gps0", "bolun_lumia_gps", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_LTE, "modem0", "bolun_lumia_lte", profile->display_name, profile->lte_available);
    register_device(runtime, BOLUN_HAL_SIM, "sim0", "bolun_lumia_sim", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_BATTERY, "battery0", "bolun_lumia_battery", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_CHARGING, "charger0", "bolun_lumia_charging", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_SD_CARD, "sd0", "bolun_lumia_sdcard", profile->display_name, 1);
    register_device(runtime, BOLUN_HAL_SENSOR, "sensors0", "bolun_lumia_sensors", profile->display_name, 1);
    append_log(runtime, 1, "HAL", profile->display_name);
    return BOLUN_OK;
}

BolunResult bolun_runtime_list_lumia_profiles(BolunLumiaProfile *out_profiles, size_t capacity, size_t *out_count) {
    if (out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    size_t profile_count = sizeof(lumia_profiles) / sizeof(lumia_profiles[0]);
    *out_count = profile_count;
    if (out_profiles != NULL) {
        size_t count = profile_count < capacity ? profile_count : capacity;
        memcpy(out_profiles, lumia_profiles, count * sizeof(BolunLumiaProfile));
    }
    return BOLUN_OK;
}

BolunResult bolun_runtime_add_file(BolunRuntime *runtime, const char *path, size_t size_bytes, const char *content_hash) {
    if (runtime == NULL || path == NULL || path[0] == '\0' || content_hash == NULL || content_hash[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < runtime->file_count; ++index) {
        if (strcmp(runtime->files[index].path, path) == 0) {
            runtime->files[index].size_bytes = size_bytes;
            copy_text(runtime->files[index].content_hash, sizeof(runtime->files[index].content_hash), content_hash);
            runtime->files[index].deleted = 0;
            return BOLUN_OK;
        }
    }
    if (runtime->file_count >= BOLUN_MAX_FILES) {
        return BOLUN_ERROR_FULL;
    }
    BolunFileRecord *file = &runtime->files[runtime->file_count++];
    memset(file, 0, sizeof(*file));
    copy_text(file->path, sizeof(file->path), path);
    copy_text(file->content_hash, sizeof(file->content_hash), content_hash);
    file->size_bytes = size_bytes;
    return BOLUN_OK;
}

BolunResult bolun_runtime_search_files(const BolunRuntime *runtime, const char *query, BolunFileRecord *out_files, size_t capacity, size_t *out_count) {
    if (runtime == NULL || query == NULL || out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    size_t matches = 0;
    for (size_t index = 0; index < runtime->file_count; ++index) {
        if (!runtime->files[index].deleted && text_contains(runtime->files[index].path, query)) {
            if (out_files != NULL && matches < capacity) {
                out_files[matches] = runtime->files[index];
            }
            matches += 1;
        }
    }
    *out_count = matches;
    return BOLUN_OK;
}

BolunResult bolun_runtime_move_file_to_trash(BolunRuntime *runtime, const char *path) {
    if (runtime == NULL || path == NULL || path[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < runtime->file_count; ++index) {
        if (strcmp(runtime->files[index].path, path) == 0) {
            runtime->files[index].deleted = 1;
            return BOLUN_OK;
        }
    }
    return BOLUN_ERROR_NOT_FOUND;
}

BolunResult bolun_runtime_archive_file(BolunRuntime *runtime, const char *path) {
    if (runtime == NULL || path == NULL || path[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < runtime->file_count; ++index) {
        if (strcmp(runtime->files[index].path, path) == 0 && !runtime->files[index].deleted) {
            runtime->files[index].archived = 1;
            return BOLUN_OK;
        }
    }
    return BOLUN_ERROR_NOT_FOUND;
}

BolunResult bolun_runtime_check_file_integrity(const BolunRuntime *runtime, const char *path, const char *expected_hash) {
    if (runtime == NULL || path == NULL || path[0] == '\0' || expected_hash == NULL || expected_hash[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < runtime->file_count; ++index) {
        if (strcmp(runtime->files[index].path, path) == 0 && !runtime->files[index].deleted) {
            return strcmp(runtime->files[index].content_hash, expected_hash) == 0 ? BOLUN_OK : BOLUN_ERROR_INTERNAL;
        }
    }
    return BOLUN_ERROR_NOT_FOUND;
}

static void parse_ben_open_value(const char *text, const char *key, char *out_value, size_t out_capacity) {
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "%s=", key);
    const char *start = strstr(text, pattern);
    if (start == NULL) {
        copy_text(out_value, out_capacity, "");
        return;
    }
    start += strlen(pattern);
    const char *end = strchr(start, '\n');
    size_t length = end == NULL ? strlen(start) : (size_t)(end - start);
    if (length >= out_capacity) {
        length = out_capacity - 1;
    }
    memcpy(out_value, start, length);
    out_value[length] = '\0';
}

BolunResult bolun_runtime_install_bapp(BolunRuntime *runtime, const char *ben_open_text, const char *signature, BolunPackageRecord *out_package) {
    if (runtime == NULL || ben_open_text == NULL || ben_open_text[0] == '\0' || signature == NULL || signature[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (runtime->package_count >= BOLUN_MAX_PACKAGES) {
        return BOLUN_ERROR_FULL;
    }
    BolunPackageRecord package;
    memset(&package, 0, sizeof(package));
    parse_ben_open_value(ben_open_text, "id", package.package_id, sizeof(package.package_id));
    parse_ben_open_value(ben_open_text, "title", package.title, sizeof(package.title));
    parse_ben_open_value(ben_open_text, "version", package.version, sizeof(package.version));
    if (package.package_id[0] == '\0' || package.title[0] == '\0' || package.version[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    copy_text(package.signature, sizeof(package.signature), signature);
    package.installed = 1;
    package.auto_update = strstr(ben_open_text, "auto_update=true") != NULL ? 1 : 0;
    for (size_t index = 0; index < runtime->package_count; ++index) {
        if (strcmp(runtime->packages[index].package_id, package.package_id) == 0) {
            runtime->packages[index] = package;
            if (out_package != NULL) {
                *out_package = package;
            }
            return BOLUN_OK;
        }
    }
    runtime->packages[runtime->package_count++] = package;
    if (out_package != NULL) {
        *out_package = package;
    }
    append_log(runtime, 1, "Package", "BAPP installed from Ben_open.txt");
    return BOLUN_OK;
}

BolunResult bolun_runtime_list_packages(const BolunRuntime *runtime, BolunPackageRecord *out_packages, size_t capacity, size_t *out_count) {
    if (runtime == NULL || out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    *out_count = runtime->package_count;
    if (out_packages != NULL) {
        size_t count = runtime->package_count < capacity ? runtime->package_count : capacity;
        memcpy(out_packages, runtime->packages, count * sizeof(BolunPackageRecord));
    }
    return BOLUN_OK;
}

BolunResult bolun_runtime_security_check(const BolunRuntime *runtime, uint32_t pid, uint32_t required_permission) {
    if (runtime == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    const BolunProcessDescriptor *process = find_process_const(runtime, pid);
    if (process == NULL) {
        return BOLUN_ERROR_NOT_FOUND;
    }
    if (required_permission == 0 || (process->permissions & required_permission) == required_permission) {
        return BOLUN_OK;
    }
    return BOLUN_ERROR_INVALID_ARGUMENT;
}
