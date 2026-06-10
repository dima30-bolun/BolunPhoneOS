#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_installer_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "installer",
        "App Installer",
        "Applications",
        "install queue, compatibility checks, signature validation, rollback, safe removal notices",
        "Installer coordinates .bapp deployment and rollback records.",
        BOLUN_SUBSYSTEM_READY,
        5,
        5
    };
}
