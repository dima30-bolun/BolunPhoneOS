#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_usb_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "usb",
        "USB",
        "Hardware",
        "charging, debug transport, storage transport, fastboot-like flashing, accessory events",
        "USB HAL is ready for charger, debug, and recovery transports.",
        BOLUN_SUBSYSTEM_HARDWARE_OPTIONAL,
        5,
        5
    };
}
