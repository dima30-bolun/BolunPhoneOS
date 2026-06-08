#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "bolun/bolun_types.h"

namespace bolun {

struct AppRecord {
    std::string app_id;
    std::string title;
    std::string publisher;
    std::string capability;
    BolunTileSize preferred_tile_size{BOLUN_TILE_MEDIUM};
    std::uint32_t accent_argb{0xff0078d7};
};

struct TileRecord {
    std::string app_id;
    std::string title;
    std::string glyph;
    BolunTileSize size{BOLUN_TILE_MEDIUM};
    std::uint32_t accent_argb{0xff0078d7};
    std::uint32_t badge{0};
};

struct NotificationRecord {
    std::string app_id;
    std::string title;
    std::string body;
    std::uint32_t priority{1};
    std::uint64_t sequence{0};
    bool unread{true};
};

struct ContactRecord {
    std::string name;
    std::string phone;
    std::string email;
    bool favorite{false};
};

struct MessageRecord {
    std::string sender;
    std::string recipient;
    std::string body;
    std::uint64_t sequence{0};
    bool delivered{true};
};

class AppManager {
public:
    bool install(const AppRecord &app);
    std::optional<AppRecord> find(std::string_view app_id) const;
    const std::vector<AppRecord> &apps() const;

private:
    std::vector<AppRecord> apps_;
};

class TileService {
public:
    bool pin(const TileRecord &tile);
    bool set_badge(std::string_view app_id, std::uint32_t badge);
    const std::vector<TileRecord> &tiles() const;

private:
    std::vector<TileRecord> tiles_;
};

class NotificationCenter {
public:
    bool push(NotificationRecord notification);
    std::size_t unread_count() const;
    const std::vector<NotificationRecord> &items() const;

private:
    std::uint64_t next_sequence_{1};
    std::vector<NotificationRecord> items_;
};

class PeopleHub {
public:
    bool add(ContactRecord contact);
    const std::vector<ContactRecord> &contacts() const;

private:
    std::vector<ContactRecord> contacts_;
};

class MessagingService {
public:
    bool send(MessageRecord message);
    const std::vector<MessageRecord> &messages() const;

private:
    std::uint64_t next_sequence_{1};
    std::vector<MessageRecord> messages_;
};

} // namespace bolun
