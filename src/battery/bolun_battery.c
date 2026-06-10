#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_battery_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "battery",
        "Battery",
        "Hardware",
        "battery gauge, health telemetry, charger state, cycle hints, temperature events",
        "Battery HAL publishes health, charge, and thermal signals.",
        BOLUN_SUBSYSTEM_READY,
        5,
        5
    };
}
