#include "bolun_core.hpp"

#include <cstring>

namespace bolun {

void copy_text(char *destination, std::size_t capacity, const std::string &source) {
    if (capacity == 0) {
        return;
    }
    std::snprintf(destination, capacity, "%s", source.c_str());
}

AppRecord from_descriptor(const BolunAppDescriptor &descriptor) {
    return AppRecord{
        descriptor.app_id,
        descriptor.title,
        descriptor.publisher,
        descriptor.capability,
        descriptor.preferred_tile_size,
        descriptor.accent_argb,
    };
}

TileRecord tile_from_app(const AppRecord &app) {
    return TileRecord{app.app_id, app.title, app.capability, app.preferred_tile_size, app.accent_argb, 0};
}

void to_descriptor(const TileRecord &record, BolunTileDescriptor &descriptor) {
    copy_text(descriptor.app_id, sizeof(descriptor.app_id), record.app_id);
    copy_text(descriptor.title, sizeof(descriptor.title), record.title);
    copy_text(descriptor.glyph, sizeof(descriptor.glyph), record.glyph);
    descriptor.size = record.size;
    descriptor.accent_argb = record.accent_argb;
    descriptor.badge = record.badge;
}

void to_descriptor(const NotificationRecord &record, BolunNotificationDescriptor &descriptor) {
    copy_text(descriptor.app_id, sizeof(descriptor.app_id), record.app_id);
    copy_text(descriptor.title, sizeof(descriptor.title), record.title);
    copy_text(descriptor.body, sizeof(descriptor.body), record.body);
    descriptor.priority = record.priority;
    descriptor.sequence = record.sequence;
    descriptor.unread = record.unread ? 1 : 0;
}

void to_descriptor(const MessageRecord &record, BolunMessageDescriptor &descriptor) {
    copy_text(descriptor.sender, sizeof(descriptor.sender), record.sender);
    copy_text(descriptor.recipient, sizeof(descriptor.recipient), record.recipient);
    copy_text(descriptor.body, sizeof(descriptor.body), record.body);
    descriptor.sequence = record.sequence;
    descriptor.delivered = record.delivered ? 1 : 0;
}

} // namespace bolun
