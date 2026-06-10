#ifndef BOLUN_TYPES_H
#define BOLUN_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BOLUN_TEXT_CAPACITY 96
#define BOLUN_BODY_CAPACITY 256
#define BOLUN_MAX_APPS 128
#define BOLUN_MAX_TILES 128
#define BOLUN_MAX_FEATURES 320
#define BOLUN_MAX_SUBSYSTEMS 32
#define BOLUN_MAX_NOTIFICATIONS 128
#define BOLUN_MAX_CONTACTS 256
#define BOLUN_MAX_MESSAGES 512

typedef enum BolunTileSize {
    BOLUN_TILE_SMALL = 1,
    BOLUN_TILE_MEDIUM = 2,
    BOLUN_TILE_WIDE = 4,
    BOLUN_TILE_LARGE = 8
} BolunTileSize;

typedef enum BolunFeatureState {
    BOLUN_FEATURE_READY = 1,
    BOLUN_FEATURE_HARDWARE_OPTIONAL = 2,
    BOLUN_FEATURE_DEVELOPER_ONLY = 3,
    BOLUN_FEATURE_PLANNED = 4
} BolunFeatureState;

typedef enum BolunSubsystemState {
    BOLUN_SUBSYSTEM_READY = 1,
    BOLUN_SUBSYSTEM_HARDWARE_OPTIONAL = 2,
    BOLUN_SUBSYSTEM_DEVELOPER_ONLY = 3,
    BOLUN_SUBSYSTEM_PLANNED = 4
} BolunSubsystemState;

typedef struct BolunAppDescriptor {
    char app_id[BOLUN_TEXT_CAPACITY];
    char title[BOLUN_TEXT_CAPACITY];
    char publisher[BOLUN_TEXT_CAPACITY];
    char capability[BOLUN_TEXT_CAPACITY];
    BolunTileSize preferred_tile_size;
    uint32_t accent_argb;
} BolunAppDescriptor;

typedef struct BolunTileDescriptor {
    char app_id[BOLUN_TEXT_CAPACITY];
    char title[BOLUN_TEXT_CAPACITY];
    char glyph[BOLUN_TEXT_CAPACITY];
    BolunTileSize size;
    uint32_t accent_argb;
    uint32_t badge;
} BolunTileDescriptor;

typedef struct BolunFeatureDescriptor {
    char area[BOLUN_TEXT_CAPACITY];
    char name[BOLUN_TEXT_CAPACITY];
    char notes[BOLUN_BODY_CAPACITY];
    BolunFeatureState state;
} BolunFeatureDescriptor;

typedef struct BolunSubsystemDescriptor {
    char subsystem_id[BOLUN_TEXT_CAPACITY];
    char title[BOLUN_TEXT_CAPACITY];
    char category[BOLUN_TEXT_CAPACITY];
    char provided_services[BOLUN_BODY_CAPACITY];
    char boot_event[BOLUN_BODY_CAPACITY];
    BolunSubsystemState state;
    uint32_t service_count;
    uint32_t capability_count;
} BolunSubsystemDescriptor;

typedef struct BolunDeviceProfile {
    char codename[BOLUN_TEXT_CAPACITY];
    char display_name[BOLUN_TEXT_CAPACITY];
    char target_family[BOLUN_TEXT_CAPACITY];
    uint8_t nfc_available;
    uint8_t qr_code_available;
    uint8_t secure_boot_required;
    uint8_t developer_mode_enabled;
} BolunDeviceProfile;

typedef struct BolunStoreAppRemovalPolicy {
    char app_id[BOLUN_TEXT_CAPACITY];
    char reason[BOLUN_BODY_CAPACITY];
    uint8_t removed_from_store;
    uint8_t security_removal;
    uint8_t installed_copy_kept;
    uint8_t launch_allowed;
} BolunStoreAppRemovalPolicy;

typedef struct BolunNotificationDescriptor {
    char app_id[BOLUN_TEXT_CAPACITY];
    char title[BOLUN_TEXT_CAPACITY];
    char body[BOLUN_BODY_CAPACITY];
    uint32_t priority;
    uint64_t sequence;
    uint8_t unread;
} BolunNotificationDescriptor;

typedef struct BolunContactDescriptor {
    char name[BOLUN_TEXT_CAPACITY];
    char phone[BOLUN_TEXT_CAPACITY];
    char email[BOLUN_TEXT_CAPACITY];
    uint8_t favorite;
} BolunContactDescriptor;

typedef struct BolunMessageDescriptor {
    char sender[BOLUN_TEXT_CAPACITY];
    char recipient[BOLUN_TEXT_CAPACITY];
    char body[BOLUN_BODY_CAPACITY];
    uint64_t sequence;
    uint8_t delivered;
} BolunMessageDescriptor;

typedef struct BolunSystemStatus {
    char device_name[BOLUN_TEXT_CAPACITY];
    char company[BOLUN_TEXT_CAPACITY];
    uint8_t booted;
    size_t app_count;
    size_t tile_count;
    size_t feature_count;
    size_t subsystem_count;
    size_t unread_notifications;
    uint8_t nfc_available;
    uint8_t qr_code_available;
} BolunSystemStatus;

#ifdef __cplusplus
}
#endif

#endif
