#include "bolun_core.hpp"

namespace bolun {

bool AppManager::install(const AppRecord &app) {
    if (app.app_id.empty() || apps_.size() >= BOLUN_MAX_APPS) {
        return false;
    }
    auto existing = std::find_if(apps_.begin(), apps_.end(), [&](const AppRecord &record) {
        return record.app_id == app.app_id;
    });
    if (existing != apps_.end()) {
        *existing = app;
        return true;
    }
    apps_.push_back(app);
    return true;
}

std::optional<AppRecord> AppManager::find(std::string_view app_id) const {
    auto found = std::find_if(apps_.begin(), apps_.end(), [&](const AppRecord &record) {
        return record.app_id == app_id;
    });
    if (found == apps_.end()) {
        return std::nullopt;
    }
    return *found;
}

const std::vector<AppRecord> &AppManager::apps() const {
    return apps_;
}

} // namespace bolun
