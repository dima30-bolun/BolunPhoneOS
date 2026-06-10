#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_services_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "services",
        "System Services",
        "System",
        "service registry, IPC endpoints, timers, lifecycle, logging",
        "System service registry coordinates boot and app-facing services.",
        BOLUN_SUBSYSTEM_READY,
        5,
        5
    };
}
