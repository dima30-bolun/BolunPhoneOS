#ifndef BOLUN_TYPES_H
#define BOLUN_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BOLUN_TEXT_CAPACITY 96
#define BOLUN_BODY_CAPACITY 256
#define BOLUN_MAX_APPS 64
#define BOLUN_MAX_TILES 96
#define BOLUN_MAX_NOTIFICATIONS 128
#define BOLUN_MAX_CONTACTS 256
#define BOLUN_MAX_MESSAGES 512

typedef enum BolunTileSize {
    BOLUN_TILE_SMALL = 1,
    BOLUN_TILE_MEDIUM = 2,
    BOLUN_TILE_WIDE = 4,
    BOLUN_TILE_LARGE = 8
} BolunTileSize;

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
    size_t unread_notifications;
} BolunSystemStatus;

#ifdef __cplusplus
}
#endif

#endif
