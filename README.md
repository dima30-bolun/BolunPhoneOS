# BolunPhoneOS

BolunPhoneOS is an original, clean-room educational phone operating-system prototype branded as **Bolun Phone** by **dima30**. It uses the same broad product idea as classic tile-based phone systems—lock screen, live tiles, app list, notifications, contacts, messages, store, media, maps, settings—but the code, names, and assets are Bolun-owned placeholders rather than proprietary Microsoft Windows Phone source or artwork.

> This repository is still a prototype and hardware-abstraction demo, not a flashable production ROM. The new Bolun Phone 1.0 catalog records the kernel/service/app contracts needed for Lumia-class ARM phones, Redmi 9A, Redmi 9C NFC, and Bolun Play targets so that device-specific ports can implement the real drivers behind the stable ABI.

## What is included

- **C kernel API** in `src/kernel/c/` for boot, shutdown, app registry, launch, notifications, contacts, messaging, device profiles, subsystem registry, the Bolun Phone 1.0 feature manifest, and Dima30 Store removed-app policy.
- **Separate C subsystem modules** under `src/ai/`, `src/apps/`, `src/audio/`, `src/bapp/`, `src/battery/`, `src/bid/`, `src/bluetooth/`, `src/camera/`, `src/cloud/`, `src/crypto/`, `src/drivers/`, `src/filesystem/`, `src/graphics/`, `src/input/`, `src/installer/`, `src/network/`, `src/notifications/`, `src/permissions/`, `src/phone/`, `src/power/`, `src/security/`, `src/services/`, `src/shell/`, `src/store/`, `src/themes/`, `src/update/`, `src/usb/`, `src/vpn/`, `src/wifi/`, and `src/youtube/`. These are still HAL/service contracts, but they are now compiled code modules instead of a single text-only catalog.
- **C++ service layer** in `src/kernel/cpp/` for native app, tile, notification, people, and messaging services.
- **Assembler HAL stubs** in `src/boot/asm/` for a boot banner, CPU probe flag, and interrupt placeholder.
- **C# shell prototype** in `src/ui/csharp/BolunPhone.Shell/` that renders a Bolun lock screen, tile start screen, and all-apps list.
- **CMake build** for the native C/C++/ASM core and tests.

## Bolun Phone 1.0 platform catalog

The native kernel now boots real subsystem descriptors plus a Bolun Phone 1.0 feature catalog with:

- Kernel services: process scheduler, multitasking, memory management, virtual memory, IPC, syscalls, timers, interrupts, power saving, and event logging.
- Boot services: bootloader, Secure Boot, Recovery Mode, Fastboot-like mode, Developer Mode, OTA updates, and rollback metadata.
- Hardware contracts: display, touch, buttons, haptics, motion sensors, proximity/light sensors, GPS, NFC, Bluetooth, Wi-Fi, cellular/SIM, USB, charging, battery, camera/flash, audio, headset jack, and SD card.
- Graphics and shell contracts: GPU acceleration, double buffering, 60 FPS animations, dark/light/adaptive themes, live tiles, widgets, and optional Always-On Display.
- Camera/audio/phone/network/security/AI/store/cloud/filesystem/performance/extras feature descriptors matching the Bolun Phone 1.0 request.
- Developer Tools folder entries for package management, `.bapp`, `Ben_open.txt`, Python, Lua, JavaScript, TinyCC, C++, Rust, C#, HTML5, WebAssembly, Bolun APIs/SDK, debug libraries, docs, signing, build tools, and optional API emulator.

## Built-in applications

The C kernel and C# shell catalog include the built-in application baseline for Bolun Phone 1.0:

- Communication: Phone, Contacts, Messages, Email, Video Calls, Call Log.
- Internet: Browser, YouTube, Dima30 Store, Downloads, VPN.
- Multimedia: Camera, Gallery, Video, Music, Voice Recorder, FM Radio.
- Office: Notes, Calendar, Calculator, Clock, Alarm, Timer, Stopwatch, Documents, PDF Reader.
- Files: File Manager, Archiver, Recycle Bin, Downloads.
- System: Settings, System Update, Security, B-ID, Bolun Account, Backup, Restore, App Permissions, Storage, Battery, Memory, Network, Bluetooth, Wi-Fi, VPN, About Device.
- Maps/navigation: Maps, Navigator, Compass, Weather.
- AI: Bolun AI, AI Camera, AI Translator, AI OCR, AI Search, AI Photo Editor.
- Data transfer: Bolun Share, Bolun Link, Bolun Cast.
- Security: Passwords, Certificates, App Check, Bolun Defender.
- Developer-only tools: Developer Tools, Package Manager, Log Viewer, API Explorer, Hardware Test, System Diagnostics.
- Extras: Keyboard, Wallpapers, Themes, Emoji, QR Scanner, Document Scanner, Notification Center, Control Center, Voice Assistant, Search, Widgets, Live Tiles, Emergency Help, Feedback, Licenses, User Guide.

## Device and payment behavior

`BolunKernelConfig.device_codename` selects the target profile:

- `redmi-9a` and non-NFC targets expose QR code payments/scanning only.
- `redmi-9c-nfc`, NFC Lumia variants, and `bolun-play` expose both NFC and QR code paths.
- QR code support is always enabled so every target has a fallback when NFC hardware is missing.

Dima30 Store removed-app policy is explicit: already-installed apps are not deleted automatically. If the store removal is not a security removal, the installed copy can still launch and the server reason is shown. If the removal is a security removal, launch is blocked and the official server reason is shown.

## Architecture

```text
include/bolun/                 Public C ABI headers
src/ai/                       Bolun AI subsystem descriptor
src/apps/                     Built-in apps subsystem descriptor
src/audio/                    Audio subsystem descriptor
src/bapp/                     .bapp installer/developer tooling subsystem descriptor
src/battery/                  Battery subsystem descriptor
src/bid/                      B-ID identity subsystem descriptor
src/bluetooth/                Bluetooth subsystem descriptor
src/boot/asm/                 Educational boot/HAL assembler stubs
src/camera/                   Camera subsystem descriptor
src/cloud/                    Bolun Cloud subsystem descriptor
src/crypto/                   Crypto subsystem descriptor
src/drivers/                  Hardware driver subsystem descriptor
src/filesystem/               File-system subsystem descriptor
src/graphics/                 Graphics/shell subsystem descriptor
src/input/                    Input subsystem descriptor
src/installer/                App installer subsystem descriptor
src/kernel/c/                 Kernel lifecycle, app catalog, device profile, feature manifest
src/kernel/cpp/               Native service implementations
src/network/                  Network/VPN subsystem descriptor
src/notifications/            Notification subsystem descriptor
src/permissions/              Permission subsystem descriptor
src/phone/                    Telephony subsystem descriptor
src/power/                    Power/battery subsystem descriptor
src/security/                 Security subsystem descriptor
src/services/                 System service registry subsystem descriptor
src/shell/                    Shell subsystem descriptor
src/store/                    Dima30 Store subsystem descriptor
src/themes/                   Theme subsystem descriptor
src/update/                   OTA/recovery subsystem descriptor
src/usb/                      USB subsystem descriptor
src/vpn/                      VPN subsystem descriptor
src/wifi/                     Wi-Fi subsystem descriptor
src/youtube/                  YouTube app-wrapper subsystem descriptor
src/ui/csharp/BolunPhone.Shell C# lock screen, start screen, app list demo
tests/cpp/                    Native core tests
```

## Build native core

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/bolun_demo
```

Expected demo output begins with:

```text
Bolun Phone 1.0: 29 subsystems, 183 features, NFC=yes, QR=yes
Bolun Phone subsystems:
- Bolun AI: assistant, chat, translation, OCR, search, summaries, photo/video enhancement, dictation, keyboard, anti-spam, battery/memory optimization
```

## Run C# shell prototype

If the .NET SDK is installed:

```bash
dotnet run --project src/ui/csharp/BolunPhone.Shell/BolunPhone.Shell.csproj
```

The C# shell is intentionally original: it does not use Microsoft Store, Office, Edge, Lumia, Nokia, or Windows icons. It renders Bolun equivalents such as **Dima30 Store**, **Bolun AI**, **Bolun Share**, **Bolun Mail/Email**, **Music**, **Weather**, **Maps**, and the Developer Tools folder.

## Legal note

This repository does **not** contain Windows Phone 8.1 source code and does **not** claim binary/API compatibility with Windows Phone, Redmi firmware, Android, or proprietary Lumia firmware. User-provided screenshots or feature requests are treated only as product requirements for an original Bolun-themed prototype.
