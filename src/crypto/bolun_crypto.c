#include "bolun/bolun_subsystems.h"

BolunSubsystemDescriptor bolun_crypto_subsystem(void) {
    return (BolunSubsystemDescriptor){
        "crypto",
        "Crypto",
        "Security",
        "memory encryption, file encryption, signatures, certificates, integrity hashes",
        "Crypto provider supplies hashing, signing, and encryption primitives.",
        BOLUN_SUBSYSTEM_READY,
        5,
        5
    };
}
