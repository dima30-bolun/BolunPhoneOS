#include "bolun_core.hpp"

namespace bolun {

bool MessagingService::send(MessageRecord message) {
    if (message.sender.empty() || message.recipient.empty() || messages_.size() >= BOLUN_MAX_MESSAGES) {
        return false;
    }
    message.sequence = next_sequence_++;
    message.delivered = true;
    messages_.push_back(std::move(message));
    return true;
}

const std::vector<MessageRecord> &MessagingService::messages() const {
    return messages_;
}

} // namespace bolun
