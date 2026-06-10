#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_apps_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "apps",
        "Built-in Apps",
        "Applications",
        "phone, contacts, messages, camera, gallery, music, browser, documents, settings, tools",
        "Built-in app registry validates launch IDs and live-tile metadata.",
        BOLUN_SUBSYSTEM_READY,
        10,
        80
    };
}
