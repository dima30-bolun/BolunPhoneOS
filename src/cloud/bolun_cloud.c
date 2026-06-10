#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_cloud_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "cloud",
        "Bolun Cloud",
        "Cloud",
        "sync, backups, cloud storage, settings sync, app sync",
        "Cloud sync contracts are available for account-enabled devices.",
        BOLUN_SUBSYSTEM_READY,
        5,
        5
    };
}
