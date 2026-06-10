#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_drivers_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "drivers",
        "Device Drivers",
        "Hardware",
        "display, touch, buttons, vibration, sensors, GPS, NFC, Bluetooth, Wi-Fi, modem, SIM, USB, charging, battery, camera, audio, SD card",
        "Hardware abstraction table is ready for device-specific ports.",
        BOLUN_SUBSYSTEM_HARDWARE_OPTIONAL,
        18,
        21
    };
}
