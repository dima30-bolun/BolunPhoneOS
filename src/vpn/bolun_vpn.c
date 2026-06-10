#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_vpn_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "vpn",
        "VPN",
        "Connectivity",
        "VPN framework, provider API, routes, DNS policy, per-app tunnels",
        "VPN broker registers provider APIs and routing policy.",
        BOLUN_SUBSYSTEM_READY,
        5,
        5
    };
}
