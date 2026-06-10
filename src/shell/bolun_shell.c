#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_shell_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "shell",
        "Shell",
        "Shell",
        "lock screen, start screen, app list, control center, live tiles, widgets",
        "Shell contract binds tiles, app list, notifications, and themes.",
        BOLUN_SUBSYSTEM_READY,
        6,
        6
    };
}
