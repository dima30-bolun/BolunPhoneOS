#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_power_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "power",
        "Power and Battery",
        "System",
        "battery telemetry, charging policy, thermal hints, power saving, AI battery optimization, idle scheduling",
        "Power manager exposes battery, charger, thermal, and idle policies.",
        BOLUN_SUBSYSTEM_READY,
        6,
        6
    };
}
