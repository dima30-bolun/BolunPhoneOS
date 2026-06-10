#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_filesystem_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "filesystem",
        "File System",
        "Storage",
        "journaling, search, recycle bin, archiver, sharing, encryption, integrity checks",
        "Journaled virtual filesystem services are mounted.",
        BOLUN_SUBSYSTEM_READY,
        7,
        7
    };
}
