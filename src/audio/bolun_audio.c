#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_audio_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "audio",
        "Audio",
        "Media",
        "voice recorder, AI noise suppression, voice enhancement, echo cancellation, equalizer, spatial audio, Bluetooth audio",
        "Audio mixer and capture/playback contracts are available.",
        BOLUN_SUBSYSTEM_READY,
        7,
        7
    };
}
