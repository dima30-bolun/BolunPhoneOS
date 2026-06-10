#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_bapp_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "bapp",
        "BAPP Installer",
        "Applications",
        "package manager, .bapp parser, Ben_open.txt metadata, signature verification, build-tool handoff",
        "BAPP package runtime accepts signed install requests.",
        BOLUN_SUBSYSTEM_DEVELOPER_ONLY,
        5,
        5
    };
}
