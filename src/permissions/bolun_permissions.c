#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_permissions_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "permissions",
        "Permissions",
        "Security",
        "runtime grants, capability declarations, prompts, audit log, sandbox binding",
        "Permission manager evaluates capabilities before service access.",
        BOLUN_SUBSYSTEM_READY,
        5,
        5
    };
}
