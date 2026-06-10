#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_youtube_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "youtube",
        "YouTube App Wrapper",
        "Applications",
        "built-in video portal tile, streaming handoff, account link, safe web launch",
        "YouTube built-in app wrapper is registered as a removable system app.",
        BOLUN_SUBSYSTEM_READY,
        4,
        4
    };
}
