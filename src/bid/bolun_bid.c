#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_bid_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "bid",
        "B-ID",
        "Identity",
        "Bolun Account sign-in, local identity token, credential binding, backup identity",
        "B-ID identity broker is available for system and store services.",
        BOLUN_SUBSYSTEM_READY,
        4,
        4
    };
}
