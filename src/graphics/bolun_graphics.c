#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_graphics_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "graphics",
        "Graphics and Shell",
        "Graphics",
        "GPU acceleration, double buffering, 60 FPS animations, dark/light themes, live tiles, widgets, adaptive themes",
        "Compositor, theme, tile, and widget service contracts are active.",
        BOLUN_SUBSYSTEM_READY,
        8,
        9
    };
}
