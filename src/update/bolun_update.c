#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_update_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "update",
        "System Update",
        "System",
        "OTA updates, recovery mode, fastboot-like mode, rollback metadata, integrity verification, developer flashing transport",
        "Update manager supports OTA staging and rollback metadata.",
        BOLUN_SUBSYSTEM_READY,
        6,
        6
    };
}
