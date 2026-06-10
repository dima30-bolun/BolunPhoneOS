#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_wifi_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "wifi",
        "Wi-Fi",
        "Connectivity",
        "scan, connect, DHCP, IPv6, power save, hotspot hooks",
        "Wi-Fi manager exposes client-network configuration hooks.",
        BOLUN_SUBSYSTEM_HARDWARE_OPTIONAL,
        6,
        6
    };
}
