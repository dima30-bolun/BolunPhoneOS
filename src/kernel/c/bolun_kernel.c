#include "bolun/bolun_boot.h"
#include "bolun/bolun_kernel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BolunKernel {
    BolunKernelConfig config;
    char device_name[BOLUN_TEXT_CAPACITY];
    char company[BOLUN_TEXT_CAPACITY];
    uint8_t booted;
    BolunAppDescriptor apps[BOLUN_MAX_APPS];
    size_t app_count;
    BolunTileDescriptor tiles[BOLUN_MAX_TILES];
    size_t tile_count;
    BolunNotificationDescriptor notifications[BOLUN_MAX_NOTIFICATIONS];
    size_t notification_count;
    BolunContactDescriptor contacts[BOLUN_MAX_CONTACTS];
    size_t contact_count;
    BolunMessageDescriptor messages[BOLUN_MAX_MESSAGES];
    size_t message_count;
    uint64_t next_notification_sequence;
    uint64_t next_message_sequence;
};

static void copy_text(char *destination, size_t capacity, const char *source) {
    if (capacity == 0) {
        return;
    }
    snprintf(destination, capacity, "%s", source == NULL ? "" : source);
}

static BolunAppDescriptor app(const char *id, const char *title, const char *capability, BolunTileSize size, uint32_t accent) {
    BolunAppDescriptor descriptor;
    memset(&descriptor, 0, sizeof(descriptor));
    copy_text(descriptor.app_id, sizeof(descriptor.app_id), id);
    copy_text(descriptor.title, sizeof(descriptor.title), title);
    copy_text(descriptor.publisher, sizeof(descriptor.publisher), "dima30");
    copy_text(descriptor.capability, sizeof(descriptor.capability), capability);
    descriptor.preferred_tile_size = size;
    descriptor.accent_argb = accent;
    return descriptor;
}

static BolunResult install_default_apps(BolunKernel *kernel) {
    const BolunAppDescriptor defaults[] = {
        app("phone", "Phone", "CALL", BOLUN_TILE_MEDIUM, 0xff1f78ff),
        app("messages", "Messages", "SMS", BOLUN_TILE_MEDIUM, 0xff9acd4f),
        app("people", "People", "PEOPLE", BOLUN_TILE_WIDE, 0xff2672ec),
        app("weather", "Weather", "SUN", BOLUN_TILE_WIDE, 0xff0078d7),
        app("photos", "Photos", "PHOTO", BOLUN_TILE_WIDE, 0xff315f8d),
        app("bolun-store", "Bolun Store", "BAG", BOLUN_TILE_MEDIUM, 0xff188038),
        app("music", "Music", "NOTE", BOLUN_TILE_MEDIUM, 0xff2b579a),
        app("mail", "Bolun Mail", "MAIL", BOLUN_TILE_MEDIUM, 0xff0078d7),
        app("news", "News", "NEWS", BOLUN_TILE_WIDE, 0xffd13438),
        app("maps", "Maps", "MAP", BOLUN_TILE_MEDIUM, 0xff315f8d),
        app("settings", "Settings", "GEAR", BOLUN_TILE_SMALL, 0xff2d2d30),
    };

    for (size_t index = 0; index < sizeof(defaults) / sizeof(defaults[0]); ++index) {
        BolunResult result = bolun_kernel_install_app(kernel, &defaults[index]);
        if (result != BOLUN_OK) {
            return result;
        }
    }
    return BOLUN_OK;
}

const char *bolun_result_message(BolunResult result) {
    switch (result) {
    case BOLUN_OK:
        return "ok";
    case BOLUN_ERROR_INVALID_ARGUMENT:
        return "invalid argument";
    case BOLUN_ERROR_NOT_BOOTED:
        return "kernel is not booted";
    case BOLUN_ERROR_NOT_FOUND:
        return "not found";
    case BOLUN_ERROR_FULL:
        return "storage is full";
    default:
        return "internal error";
    }
}

BolunKernelConfig bolun_kernel_default_config(void) {
    BolunKernelConfig config;
    config.device_name = "Bolun Phone";
    config.company = "dima30";
    config.default_accent_argb = 0xff0078d7;
    return config;
}

BolunResult bolun_kernel_create(const BolunKernelConfig *config, BolunKernel **out_kernel) {
    if (out_kernel == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    BolunKernel *kernel = calloc(1, sizeof(BolunKernel));
    if (kernel == NULL) {
        return BOLUN_ERROR_INTERNAL;
    }
    BolunKernelConfig default_config = bolun_kernel_default_config();
    if (config == NULL) {
        config = &default_config;
    }
    kernel->config = *config;
    copy_text(kernel->device_name, sizeof(kernel->device_name), config->device_name == NULL ? default_config.device_name : config->device_name);
    copy_text(kernel->company, sizeof(kernel->company), config->company == NULL ? default_config.company : config->company);
    kernel->next_notification_sequence = 1;
    kernel->next_message_sequence = 1;
    *out_kernel = kernel;
    return BOLUN_OK;
}

void bolun_kernel_destroy(BolunKernel *kernel) {
    free(kernel);
}

BolunResult bolun_kernel_boot(BolunKernel *kernel) {
    if (kernel == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (!kernel->booted) {
        BolunResult result = install_default_apps(kernel);
        if (result != BOLUN_OK) {
            return result;
        }
        BolunNotificationDescriptor notification;
        memset(&notification, 0, sizeof(notification));
        copy_text(notification.app_id, sizeof(notification.app_id), "system");
        copy_text(notification.title, sizeof(notification.title), "Welcome");
        snprintf(notification.body, sizeof(notification.body), "%s by %s | %s | cpu=0x%llx", kernel->device_name, kernel->company, bolun_boot_banner(), (unsigned long long)bolun_cpu_probe());
        bolun_kernel_push_notification(kernel, &notification);
        kernel->booted = 1;
    }
    return BOLUN_OK;
}

BolunResult bolun_kernel_shutdown(BolunKernel *kernel) {
    if (kernel == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    kernel->booted = 0;
    return BOLUN_OK;
}

BolunResult bolun_kernel_get_status(const BolunKernel *kernel, BolunSystemStatus *out_status) {
    if (kernel == NULL || out_status == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    memset(out_status, 0, sizeof(*out_status));
    copy_text(out_status->device_name, sizeof(out_status->device_name), kernel->device_name);
    copy_text(out_status->company, sizeof(out_status->company), kernel->company);
    out_status->booted = kernel->booted;
    out_status->app_count = kernel->app_count;
    out_status->tile_count = kernel->tile_count;
    for (size_t index = 0; index < kernel->notification_count; ++index) {
        if (kernel->notifications[index].unread) {
            ++out_status->unread_notifications;
        }
    }
    return BOLUN_OK;
}

BolunResult bolun_kernel_install_app(BolunKernel *kernel, const BolunAppDescriptor *app_descriptor) {
    if (kernel == NULL || app_descriptor == NULL || app_descriptor->app_id[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (kernel->app_count >= BOLUN_MAX_APPS || kernel->tile_count >= BOLUN_MAX_TILES) {
        return BOLUN_ERROR_FULL;
    }
    kernel->apps[kernel->app_count++] = *app_descriptor;
    BolunTileDescriptor tile;
    memset(&tile, 0, sizeof(tile));
    copy_text(tile.app_id, sizeof(tile.app_id), app_descriptor->app_id);
    copy_text(tile.title, sizeof(tile.title), app_descriptor->title);
    copy_text(tile.glyph, sizeof(tile.glyph), app_descriptor->capability);
    tile.size = app_descriptor->preferred_tile_size;
    tile.accent_argb = app_descriptor->accent_argb;
    kernel->tiles[kernel->tile_count++] = tile;
    return BOLUN_OK;
}

BolunResult bolun_kernel_launch_app(BolunKernel *kernel, const char *app_id, char *out_message, size_t out_message_size) {
    if (kernel == NULL || app_id == NULL || out_message == NULL || out_message_size == 0) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (!kernel->booted) {
        return BOLUN_ERROR_NOT_BOOTED;
    }
    for (size_t index = 0; index < kernel->app_count; ++index) {
        if (strcmp(kernel->apps[index].app_id, app_id) == 0) {
            snprintf(out_message, out_message_size, "%s is ready on %s", kernel->apps[index].title, kernel->device_name);
            return BOLUN_OK;
        }
    }
    return BOLUN_ERROR_NOT_FOUND;
}

BolunResult bolun_kernel_list_tiles(const BolunKernel *kernel, BolunTileDescriptor *out_tiles, size_t capacity, size_t *out_count) {
    if (kernel == NULL || out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    *out_count = kernel->tile_count;
    if (out_tiles == NULL) {
        return BOLUN_OK;
    }
    size_t count = kernel->tile_count < capacity ? kernel->tile_count : capacity;
    memcpy(out_tiles, kernel->tiles, count * sizeof(BolunTileDescriptor));
    return BOLUN_OK;
}

BolunResult bolun_kernel_push_notification(BolunKernel *kernel, const BolunNotificationDescriptor *notification) {
    if (kernel == NULL || notification == NULL || notification->title[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (kernel->notification_count >= BOLUN_MAX_NOTIFICATIONS) {
        return BOLUN_ERROR_FULL;
    }
    BolunNotificationDescriptor stored = *notification;
    stored.sequence = kernel->next_notification_sequence++;
    stored.unread = 1;
    kernel->notifications[kernel->notification_count++] = stored;
    return BOLUN_OK;
}

BolunResult bolun_kernel_list_notifications(const BolunKernel *kernel, BolunNotificationDescriptor *out_notifications, size_t capacity, size_t *out_count) {
    if (kernel == NULL || out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    *out_count = kernel->notification_count;
    if (out_notifications != NULL) {
        size_t count = kernel->notification_count < capacity ? kernel->notification_count : capacity;
        memcpy(out_notifications, kernel->notifications, count * sizeof(BolunNotificationDescriptor));
    }
    return BOLUN_OK;
}

BolunResult bolun_kernel_add_contact(BolunKernel *kernel, const BolunContactDescriptor *contact) {
    if (kernel == NULL || contact == NULL || contact->name[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (kernel->contact_count >= BOLUN_MAX_CONTACTS) {
        return BOLUN_ERROR_FULL;
    }
    kernel->contacts[kernel->contact_count++] = *contact;
    return BOLUN_OK;
}

BolunResult bolun_kernel_send_message(BolunKernel *kernel, const BolunMessageDescriptor *message) {
    if (kernel == NULL || message == NULL || message->sender[0] == '\0' || message->recipient[0] == '\0') {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    if (kernel->message_count >= BOLUN_MAX_MESSAGES) {
        return BOLUN_ERROR_FULL;
    }
    BolunMessageDescriptor stored = *message;
    stored.sequence = kernel->next_message_sequence++;
    stored.delivered = 1;
    kernel->messages[kernel->message_count++] = stored;
    return BOLUN_OK;
}

BolunResult bolun_kernel_list_messages(const BolunKernel *kernel, BolunMessageDescriptor *out_messages, size_t capacity, size_t *out_count) {
    if (kernel == NULL || out_count == NULL) {
        return BOLUN_ERROR_INVALID_ARGUMENT;
    }
    *out_count = kernel->message_count;
    if (out_messages != NULL) {
        size_t count = kernel->message_count < capacity ? kernel->message_count : capacity;
        memcpy(out_messages, kernel->messages, count * sizeof(BolunMessageDescriptor));
    }
    return BOLUN_OK;
}
