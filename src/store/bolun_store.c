#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_store_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "store",
        "Dima30 Store",
        "Applications",
        "install, update, auto-update, ratings, reviews, security checks, certificates, categories, search, rollback, compatibility, removal reasons",
        "Store policy engine is ready for signed apps and removal decisions.",
        BOLUN_SUBSYSTEM_READY,
        12,
        16
    };
}
