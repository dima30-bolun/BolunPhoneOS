#include "bolun_system.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void copy_text(char *dst, size_t cap, const char *src) {
    if (cap == 0) return;
    snprintf(dst, cap, "%s", src ? src : "");
}

static int valid_cpu(Bolun10Cpu cpu) {
    return cpu == BOLUN10_ARMV7 || cpu == BOLUN10_ARM64 || cpu == BOLUN10_X86 || cpu == BOLUN10_X86_64;
}

static Bolun10Process *find_process(Bolun10System *system, uint32_t pid) {
    for (size_t i = 0; i < system->process_count; ++i) {
        if (system->processes[i].pid == pid && system->processes[i].state != BOLUN10_PROC_STOPPED) return &system->processes[i];
    }
    return NULL;
}

static const Bolun10Process *find_process_const(const Bolun10System *system, uint32_t pid) {
    for (size_t i = 0; i < system->process_count; ++i) {
        if (system->processes[i].pid == pid && system->processes[i].state != BOLUN10_PROC_STOPPED) return &system->processes[i];
    }
    return NULL;
}

uint32_t bolun10_crc32_text(const char *text) {
    uint32_t crc = 2166136261u;
    if (text == NULL) return crc;
    while (*text) {
        crc ^= (unsigned char)*text++;
        crc *= 16777619u;
    }
    return crc;
}

int bolun10_log(Bolun10System *system, uint32_t severity, const char *area, const char *message) {
    if (!system || !area || !message || area[0] == '\0' || message[0] == '\0') return -1;
    if (system->log_count == BOLUN10_MAX_LOGS) {
        memmove(system->logs, system->logs + 1, (BOLUN10_MAX_LOGS - 1) * sizeof(system->logs[0]));
        system->log_count = BOLUN10_MAX_LOGS - 1;
    }
    Bolun10Log *log = &system->logs[system->log_count++];
    log->severity = severity;
    copy_text(log->area, sizeof(log->area), area);
    copy_text(log->message, sizeof(log->message), message);
    return 0;
}

int bolun10_system_init(Bolun10System *system, Bolun10Cpu cpu, const char *device, size_t total_memory) {
    if (!system || !valid_cpu(cpu) || !device || device[0] == '\0') return -1;
    memset(system, 0, sizeof(*system));
    system->cpu = cpu;
    copy_text(system->device, sizeof(system->device), device);
    system->next_pid = 100;
    system->next_block = 1;
    system->next_socket = 1;
    system->next_window = 1;
    system->next_message = 1;
    system->total_memory = total_memory ? total_memory : 128u * 1024u * 1024u;
    return bolun10_log(system, 1, "system", "initialized");
}

int bolun10_boot_prepare(Bolun10System *system, const char *target, uint32_t component_count) {
    if (!system || !target || target[0] == '\0' || component_count == 0) return -1;
    copy_text(system->boot.target, sizeof(system->boot.target), target);
    system->boot.version = 1;
    system->boot.component_count = component_count;
    system->boot.image_crc = bolun10_crc32_text(target) ^ component_count;
    return bolun10_log(system, 1, "boot", "image prepared");
}

int bolun10_process_spawn(Bolun10System *system, const char *name, uint32_t permissions, size_t memory_limit, uint32_t *out_pid) {
    if (!system || !name || name[0] == '\0' || !out_pid || system->process_count == BOLUN10_MAX_PROCESSES) return -1;
    Bolun10Process *proc = &system->processes[system->process_count++];
    memset(proc, 0, sizeof(*proc));
    proc->pid = system->next_pid++;
    copy_text(proc->name, sizeof(proc->name), name);
    proc->state = BOLUN10_PROC_READY;
    proc->priority = (permissions & BOLUN10_PERM_SYSTEM) ? 3 : 1;
    proc->permissions = permissions;
    proc->memory_limit = memory_limit ? memory_limit : 4u * 1024u * 1024u;
    *out_pid = proc->pid;
    return bolun10_log(system, 1, "process", "spawned");
}

int bolun10_process_schedule(Bolun10System *system, Bolun10Process *out_process) {
    if (!system || !out_process || system->process_count == 0) return -1;
    for (size_t attempt = 0; attempt < system->process_count; ++attempt) {
        system->scheduler_cursor = (system->scheduler_cursor + 1) % system->process_count;
        Bolun10Process *candidate = &system->processes[system->scheduler_cursor];
        if (candidate->state == BOLUN10_PROC_READY || candidate->state == BOLUN10_PROC_RUNNING) {
            for (size_t i = 0; i < system->process_count; ++i) if (system->processes[i].state == BOLUN10_PROC_RUNNING) system->processes[i].state = BOLUN10_PROC_READY;
            candidate->state = BOLUN10_PROC_RUNNING;
            candidate->ticks++;
            *out_process = *candidate;
            return 0;
        }
    }
    return -1;
}

int bolun10_memory_alloc(Bolun10System *system, uint32_t pid, size_t bytes, uint32_t *out_block) {
    if (!system || !out_block || bytes == 0 || system->memory_count == BOLUN10_MAX_BLOCKS) return -1;
    Bolun10Process *proc = find_process(system, pid);
    if (!proc || proc->memory_used + bytes > proc->memory_limit) return -1;
    Bolun10MemoryBlock *block = &system->memory[system->memory_count++];
    block->id = system->next_block++;
    block->owner_pid = pid;
    block->bytes = bytes;
    block->virtual_address = 0;
    block->writable = 1;
    block->used = 1;
    proc->memory_used += bytes;
    *out_block = block->id;
    return 0;
}

int bolun10_memory_free(Bolun10System *system, uint32_t block_id) {
    if (!system || block_id == 0) return -1;
    for (size_t i = 0; i < system->memory_count; ++i) {
        Bolun10MemoryBlock *block = &system->memory[i];
        if (block->id == block_id && block->used) {
            Bolun10Process *proc = find_process(system, block->owner_pid);
            if (proc && proc->memory_used >= block->bytes) proc->memory_used -= block->bytes;
            block->used = 0;
            return 0;
        }
    }
    return -1;
}

int bolun10_bfs_create(Bolun10System *system, const char *path, uint32_t owner_pid, size_t bytes, const char *contents) {
    if (!system || !path || path[0] != '/' || !find_process(system, owner_pid) || system->file_count == BOLUN10_MAX_FILES) return -1;
    Bolun10File *file = &system->files[system->file_count++];
    memset(file, 0, sizeof(*file));
    copy_text(file->path, sizeof(file->path), path);
    file->owner_pid = owner_pid;
    file->bytes = bytes;
    file->checksum = bolun10_crc32_text(contents ? contents : path);
    file->directory = path[strlen(path) - 1] == '/';
    return 0;
}

static int manifest_value(const char *manifest, const char *key, char *out, size_t out_size) {
    size_t key_len = strlen(key);
    const char *scan = manifest;
    while (scan && *scan) {
        if (strncmp(scan, key, key_len) == 0 && scan[key_len] == '=') {
            scan += key_len + 1;
            size_t i = 0;
            while (scan[i] && scan[i] != '\n' && i + 1 < out_size) { out[i] = scan[i]; i++; }
            out[i] = '\0';
            return i > 0 ? 0 : -1;
        }
        scan = strchr(scan, '\n');
        if (scan) scan++;
    }
    return -1;
}

int bolun10_bapp_install(Bolun10System *system, const char *manifest, Bolun10Package *out_package) {
    if (!system || !manifest || system->package_count == BOLUN10_MAX_PACKAGES) return -1;
    Bolun10Package pkg;
    char version[BOLUN10_TEXT];
    memset(&pkg, 0, sizeof(pkg));
    if (manifest_value(manifest, "id", pkg.id, sizeof(pkg.id)) != 0) return -1;
    if (manifest_value(manifest, "title", pkg.title, sizeof(pkg.title)) != 0) return -1;
    if (manifest_value(manifest, "version", version, sizeof(version)) != 0) return -1;
    pkg.version = (uint32_t)strtoul(version, NULL, 10);
    pkg.permissions = (strstr(manifest, "permission=net") ? BOLUN10_PERM_NET : 0) | (strstr(manifest, "permission=camera") ? BOLUN10_PERM_CAMERA : 0);
    pkg.installed = 1;
    system->packages[system->package_count++] = pkg;
    if (out_package) *out_package = pkg;
    return bolun10_log(system, 1, "package", "installed");
}

int bolun10_security_check(const Bolun10System *system, uint32_t pid, uint32_t permission) {
    if (!system || permission == 0) return -1;
    const Bolun10Process *proc = find_process_const(system, pid);
    if (!proc) return -1;
    return (proc->permissions & permission) == permission ? 0 : -1;
}

int bolun10_ui_render_home(const Bolun10System *system, char *out_text, size_t out_size) {
    if (!system || !out_text || out_size == 0) return -1;
    int written = snprintf(out_text, out_size, "Bolun UI %s processes=%zu packages=%zu files=%zu", system->device, system->process_count, system->package_count, system->file_count);
    return written > 0 && (size_t)written < out_size ? 0 : -1;
}


int bolun10_memory_map(Bolun10System *system, uint32_t block_id, uintptr_t virtual_address, uint8_t writable) {
    if (!system || block_id == 0 || virtual_address == 0) return -1;
    for (size_t i = 0; i < system->memory_count; ++i) {
        Bolun10MemoryBlock *block = &system->memory[i];
        if (block->id == block_id && block->used) {
            block->virtual_address = virtual_address;
            block->writable = writable ? 1 : 0;
            return 0;
        }
    }
    return -1;
}

int bolun10_bfs_read(const Bolun10System *system, const char *path, Bolun10File *out_file) {
    if (!system || !path || !out_file) return -1;
    for (size_t i = 0; i < system->file_count; ++i) {
        if (strcmp(system->files[i].path, path) == 0) {
            *out_file = system->files[i];
            return 0;
        }
    }
    return -1;
}

int bolun10_ipc_send(Bolun10System *system, uint32_t from_pid, uint32_t to_pid, const char *channel, const char *body) {
    if (!system || !channel || !body || channel[0] == '\0' || body[0] == '\0' || system->message_count == BOLUN10_MAX_MESSAGES) return -1;
    if (!find_process(system, from_pid) || !find_process(system, to_pid)) return -1;
    Bolun10Message *message = &system->messages[system->message_count++];
    memset(message, 0, sizeof(*message));
    message->from_pid = from_pid;
    message->to_pid = to_pid;
    message->sequence = system->next_message++;
    copy_text(message->channel, sizeof(message->channel), channel);
    copy_text(message->body, sizeof(message->body), body);
    return 0;
}

int bolun10_ipc_receive(Bolun10System *system, uint32_t to_pid, Bolun10Message *out_message) {
    if (!system || !out_message || !find_process(system, to_pid)) return -1;
    for (size_t i = 0; i < system->message_count; ++i) {
        if (system->messages[i].to_pid == to_pid) {
            *out_message = system->messages[i];
            memmove(&system->messages[i], &system->messages[i + 1], (system->message_count - i - 1) * sizeof(system->messages[0]));
            system->message_count--;
            return 0;
        }
    }
    return -1;
}

int bolun10_driver_register(Bolun10System *system, Bolun10DriverClass type, const char *name, uint32_t unit) {
    if (!system || !name || name[0] == '\0' || system->driver_count == BOLUN10_MAX_DRIVERS) return -1;
    if (type < BOLUN10_DRIVER_DISPLAY || type > BOLUN10_DRIVER_STORAGE) return -1;
    for (size_t i = 0; i < system->driver_count; ++i) {
        if (strcmp(system->drivers[i].name, name) == 0 && system->drivers[i].unit == unit) return 0;
    }
    Bolun10Driver *driver = &system->drivers[system->driver_count++];
    memset(driver, 0, sizeof(*driver));
    driver->type = type;
    driver->unit = unit;
    driver->loaded = 1;
    driver->powered = 1;
    copy_text(driver->name, sizeof(driver->name), name);
    return bolun10_log(system, 1, "driver", "registered");
}

int bolun10_driver_power(Bolun10System *system, const char *name, uint8_t powered) {
    if (!system || !name) return -1;
    for (size_t i = 0; i < system->driver_count; ++i) {
        if (strcmp(system->drivers[i].name, name) == 0) {
            system->drivers[i].powered = powered ? 1 : 0;
            return 0;
        }
    }
    return -1;
}

int bolun10_socket_open(Bolun10System *system, uint32_t owner_pid, const char *local, const char *remote, uint32_t *out_socket) {
    if (!system || !local || !remote || !out_socket || system->socket_count == BOLUN10_MAX_SOCKETS) return -1;
    if (bolun10_security_check(system, owner_pid, BOLUN10_PERM_NET) != 0) return -1;
    Bolun10Socket *socket = &system->sockets[system->socket_count++];
    memset(socket, 0, sizeof(*socket));
    socket->id = system->next_socket++;
    socket->owner_pid = owner_pid;
    socket->open = 1;
    copy_text(socket->local, sizeof(socket->local), local);
    copy_text(socket->remote, sizeof(socket->remote), remote);
    *out_socket = socket->id;
    return 0;
}

int bolun10_socket_send(Bolun10System *system, uint32_t socket_id, const char *payload) {
    if (!system || !payload || payload[0] == '\0') return -1;
    for (size_t i = 0; i < system->socket_count; ++i) {
        Bolun10Socket *socket = &system->sockets[i];
        if (socket->id == socket_id && socket->open) {
            socket->tx_packets++;
            socket->rx_packets += (strstr(payload, "ping") != NULL) ? 1u : 0u;
            return 0;
        }
    }
    return -1;
}

int bolun10_window_create(Bolun10System *system, uint32_t owner_pid, const char *title, int width, int height, uint32_t *out_window) {
    if (!system || !title || !out_window || width <= 0 || height <= 0 || system->window_count == BOLUN10_MAX_WINDOWS) return -1;
    if (!find_process(system, owner_pid)) return -1;
    Bolun10Window *window = &system->windows[system->window_count++];
    memset(window, 0, sizeof(*window));
    window->id = system->next_window++;
    window->owner_pid = owner_pid;
    window->width = width;
    window->height = height;
    window->visible = 1;
    window->color = 0x102030u + window->id;
    copy_text(window->title, sizeof(window->title), title);
    *out_window = window->id;
    return 0;
}

int bolun10_compositor_frame(const Bolun10System *system, char *out_text, size_t out_size) {
    if (!system || !out_text || out_size == 0) return -1;
    size_t visible = 0;
    for (size_t i = 0; i < system->window_count; ++i) visible += system->windows[i].visible ? 1u : 0u;
    int written = snprintf(out_text, out_size, "frame device=%s windows=%zu low_power=%u", system->device, visible, system->low_power_mode);
    return written > 0 && (size_t)written < out_size ? 0 : -1;
}

int bolun10_power_set_low_power(Bolun10System *system, uint8_t enabled) {
    if (!system) return -1;
    system->low_power_mode = enabled ? 1 : 0;
    return bolun10_log(system, 1, "power", enabled ? "low-power enabled" : "low-power disabled");
}

int bolun10_update_apply(Bolun10System *system, const char *version, uint32_t signature) {
    if (!system || !version || version[0] == '\0') return -1;
    if ((bolun10_crc32_text(version) ^ 0xB01010u) != signature) return -1;
    system->update_generation++;
    return bolun10_log(system, 1, "update", "applied");
}

int bolun10_recovery_plan(Bolun10System *system, const char *reason) {
    if (!system || !reason || reason[0] == '\0') return -1;
    return bolun10_bfs_create(system, "/recovery/plan.bfs", system->process_count ? system->processes[0].pid : 0, strlen(reason), reason);
}

int bolun10_diagnostics_report(const Bolun10System *system, char *out_text, size_t out_size) {
    if (!system || !out_text || out_size == 0) return -1;
    int written = snprintf(out_text, out_size, "diag device=%s procs=%zu mem=%zu files=%zu pkgs=%zu drivers=%zu sockets=%zu logs=%zu updates=%u", system->device, system->process_count, system->memory_count, system->file_count, system->package_count, system->driver_count, system->socket_count, system->log_count, system->update_generation);
    return written > 0 && (size_t)written < out_size ? 0 : -1;
}
