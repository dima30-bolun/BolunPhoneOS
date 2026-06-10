#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_input_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "input",
        "Input",
        "Hardware",
        "touchscreen, buttons, keyboard, gestures, proximity wake policy",
        "Input manager merges touch, button, and keyboard events.",
        BOLUN_SUBSYSTEM_READY,
        5,
        5
    };
}
