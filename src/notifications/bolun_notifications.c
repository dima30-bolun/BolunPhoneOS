#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_notifications_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "notifications",
        "Notifications",
        "Shell",
        "notification center, badges, priorities, unread state, quick actions",
        "Notification pipeline feeds badges, live tiles, and the center.",
        BOLUN_SUBSYSTEM_READY,
        5,
        5
    };
}
