#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_themes_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "themes",
        "Themes",
        "Shell",
        "dark theme, light theme, adaptive theme, wallpapers, emoji assets",
        "Theme service publishes palette and wallpaper metadata.",
        BOLUN_SUBSYSTEM_READY,
        5,
        5
    };
}
