# Bolun HAL API

Every BSP binds devices through `BolunHalDevice`: class, name, unit, capability flags, and operation table. Display, audio, camera, sensors, storage, power, USB, Bluetooth, Wi-Fi, radio, and optional NFC modules expose probe/start/stop/ioctl calls with deterministic error codes.
