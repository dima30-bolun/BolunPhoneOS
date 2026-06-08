#include "bolun_core.hpp"

namespace bolun {

bool TileService::pin(const TileRecord &tile) {
    if (tile.app_id.empty() || tiles_.size() >= BOLUN_MAX_TILES) {
        return false;
    }
    auto existing = std::find_if(tiles_.begin(), tiles_.end(), [&](const TileRecord &record) {
        return record.app_id == tile.app_id;
    });
    if (existing != tiles_.end()) {
        *existing = tile;
        return true;
    }
    tiles_.push_back(tile);
    return true;
}

bool TileService::set_badge(std::string_view app_id, std::uint32_t badge) {
    auto existing = std::find_if(tiles_.begin(), tiles_.end(), [&](const TileRecord &record) {
        return record.app_id == app_id;
    });
    if (existing == tiles_.end()) {
        return false;
    }
    existing->badge = badge;
    return true;
}

const std::vector<TileRecord> &TileService::tiles() const {
    return tiles_;
}

} // namespace bolun
