#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_phone_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "phone",
        "Phone Stack",
        "Communication",
        "calls, SMS, MMS, contacts, call forwarding, voicemail, VoLTE hooks",
        "Telephony contracts are ready for modem-backed device ports.",
        BOLUN_SUBSYSTEM_HARDWARE_OPTIONAL,
        7,
        7
    };
}
