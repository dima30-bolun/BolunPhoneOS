#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_camera_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "camera",
        "Camera",
        "Media",
        "photo, video, HDR, night mode, panorama, burst, timer, QR scanner, document scanner, AI cleanup, EIS",
        "Camera pipeline contracts and QR/document scanning routes are online.",
        BOLUN_SUBSYSTEM_READY,
        12,
        15
    };
}
