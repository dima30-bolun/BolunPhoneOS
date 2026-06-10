#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_network_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "network",
        "Network Stack",
        "Connectivity",
        "TCP, UDP, HTTP, HTTPS, DNS, DHCP, WebSocket, IPv6, VPN framework, VPN app API",
        "Dual-stack networking and VPN provider contracts are registered.",
        BOLUN_SUBSYSTEM_READY,
        10,
        10
    };
}
