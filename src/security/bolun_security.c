#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_security_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "security",
        "Security",
        "Security",
        "memory encryption, file encryption, permissions, sandbox, signatures, secure boot, B-ID, PIN, password, pattern",
        "Security manager enforces credentials, sandbox, signatures, and encryption policy.",
        BOLUN_SUBSYSTEM_READY,
        10,
        10
    };
}
