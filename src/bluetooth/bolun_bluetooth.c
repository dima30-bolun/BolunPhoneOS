#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_bluetooth_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "bluetooth",
        "Bluetooth",
        "Connectivity",
        "pairing, device discovery, file transfer, audio routing, controller power policy",
        "Bluetooth controller contract is ready for hardware-specific adapters.",
        BOLUN_SUBSYSTEM_HARDWARE_OPTIONAL,
        5,
        5
    };
}
