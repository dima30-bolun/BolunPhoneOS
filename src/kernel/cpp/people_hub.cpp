#include "bolun_core.hpp"

namespace bolun {

bool PeopleHub::add(ContactRecord contact) {
    if (contact.name.empty() || contacts_.size() >= BOLUN_MAX_CONTACTS) {
        return false;
    }
    contacts_.push_back(std::move(contact));
    return true;
}

const std::vector<ContactRecord> &PeopleHub::contacts() const {
    return contacts_;
}

} // namespace bolun
