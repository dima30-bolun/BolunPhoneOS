#include "bolun_core.hpp"

namespace bolun {

bool NotificationCenter::push(NotificationRecord notification) {
    if (notification.title.empty() || items_.size() >= BOLUN_MAX_NOTIFICATIONS) {
        return false;
    }
    notification.sequence = next_sequence_++;
    notification.unread = true;
    items_.push_back(std::move(notification));
    return true;
}

std::size_t NotificationCenter::unread_count() const {
    return static_cast<std::size_t>(std::count_if(items_.begin(), items_.end(), [](const NotificationRecord &item) {
        return item.unread;
    }));
}

const std::vector<NotificationRecord> &NotificationCenter::items() const {
    return items_;
}

} // namespace bolun
