#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_ai_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "ai",
        "Bolun AI",
        "AI",
        "assistant, chat, translation, OCR, search, summaries, photo/video enhancement, dictation, keyboard, anti-spam, battery/memory optimization",
        "AI service graph registered for system apps and keyboard surfaces.",
        BOLUN_SUBSYSTEM_READY,
        13,
        13
    };
}
