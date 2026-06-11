# BolunPhoneOS

BolunPhoneOS is an original, clean-room educational phone operating-system prototype branded as **Bolun Phone** by **dima30**. It uses the same broad product idea as classic tile-based phone system[...]

> This repository is still a prototype and hardware-abstraction demo, not a flashable production ROM. The new Bolun Phone 1.0 catalog records the kernel/service/app contracts needed for Lumia-class[...]

## What is included

- **C kernel API** in `src/kernel/c/` for boot, shutdown, app registry, launch, notifications, contacts, messaging, device profiles, the Bolun Phone 1.0 feature manifest, and Dima30 Store removed-a[...]
- **C++ service layer** in `src/kernel/cpp/` for native app, tile, notification, people, and messaging services.
- **Assembler HAL routines** in `src/boot/asm/` for a boot banner, CPU probe flag, and interrupt placeholder.
- **C# shell prototype** in `src/ui/csharp/BolunPhone.Shell/` that renders a Bolun lock screen, tile start screen, and all-apps list.
- **CMake build** for the native C/C++/ASM core and tests.

## Bolun Phone 1.0 platform catalog

The native kernel now boots a Bolun Phone 1.0 catalog with:

- Kernel services: process scheduler, multitasking, memory management, virtual memory, IPC, syscalls, timers, interrupts, power saving, event logging, sandbox permission checks, and fixed-capacity error handling.
- Boot services: bootloader, Secure Boot, Recovery Mode, Fastboot-like mode, Developer Mode, OTA updates, and rollback metadata.
- Hardware contracts: display, touch, buttons, haptics, motion sensors, proximity/light sensors, GPS, NFC, Bluetooth, Wi-Fi, cellular/SIM, USB, charging, battery, camera/flash, audio, headset jack[...]
- Graphics and shell contracts: GPU acceleration, double buffering, 60 FPS animations, dark/light/adaptive themes, live tiles, widgets, and optional Always-On Display.
- Camera/audio/phone/network/security/AI/store/cloud/filesystem/performance/extras feature descriptors matching the Bolun Phone 1.0 request.
- Developer Tools folder entries for package management, `.bapp`, `Ben_open.txt`, Python, Lua, JavaScript, TinyCC, C++, Rust, C#, HTML5, WebAssembly, Bolun APIs/SDK, debug libraries, docs, signing[...]

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
- Extras: Keyboard, Wallpapers, Themes, Emoji, QR Scanner, Document Scanner, Notification Center, Control Center, Voice Assistant, Search, Widgets, Live Tiles, Emergency Help, Feedback, Licenses, [...]

## Device and payment behavior

`BolunKernelConfig.device_codename` selects the target profile:

- `redmi-9a` and non-NFC targets expose QR code payments/scanning only.
- `redmi-9c-nfc`, NFC Lumia variants, and `bolun-play` expose both NFC and QR code paths.
- QR code support is always enabled so every target has a fallback when NFC hardware is missing.

Dima30 Store removed-app policy is explicit: already-installed apps are not deleted automatically. If the store removal is not a security removal, the installed copy can still launch and the serve[...]

## Architecture

```text
include/bolun/                 Public C ABI headers
src/boot/asm/                  Educational boot/HAL assembler routines
src/kernel/c/                  Kernel lifecycle, app catalog, device profile, feature manifest
src/kernel/cpp/                Native service implementations
src/ui/csharp/BolunPhone.Shell C# lock screen, start screen, app list demo
tests/cpp/                     Native core tests
```

## Runtime API smoke coverage

The host-side tests now exercise the new runtime surface:

- Creates a Lumia HAL profile and routes camera commands through `bolun_runtime_hal_write`.
- Spawns sandboxed app processes and schedules them round-robin.
- Enforces memory quotas, sends IPC messages, fires timers, handles interrupts, and records system logs.
- Adds searchable virtual file records, validates integrity hashes, archives files, and moves deleted files to the trash.
- Installs a signed `.bapp` package by parsing `Ben_open.txt` keys (`id`, `title`, `version`, `auto_update`).

## Build native core

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/bolun_demo
```

Expected demo output begins with:

```text
Bolun Phone home:
[CALL] Phone
[PEOPLE] Contacts
[SMS] Messages
```

## Run C# shell prototype

If the .NET SDK is installed:

```bash
dotnet run --project src/ui/csharp/BolunPhone.Shell/BolunPhone.Shell.csproj
```

The C# shell is intentionally original: it does not use Microsoft Store, Office, Edge, Lumia, Nokia, or Windows icons. It renders Bolun equivalents such as **Dima30 Store**, **Bolun AI**, **Bolun [...]

---

## Project Review and Status

### Archive Verification

Архив проверен и полностью совпадает с предыдущей версией:
- 📁 **51 файл**
- Та же структура каталогов
- Новых модулей или подсистем не обнаружено

### Current Implementation Status

#### ✅ Реализовано

- **Загрузчик** (Assembly)
- **Базовое ядро** (C)
- **Службы ядра** (C++)
- **Консольная оболочка** (C#)
- **Архитектура проекта**
- **Тесты**

#### ❌ Пока отсутствует

- Драйверы устройств
- Камера
- Wi-Fi и Bluetooth
- Файловая система
- Настоящий графический интерфейс
- Dima30 Store
- Bolun AI
- VPN Framework
- Система разрешений
- Менеджер памяти
- Планировщик процессов
- Поддержка .bapp

### Recommended Development Roadmap

Рекомендуемый порядок развития проекта:

1. **HAL (Hardware Abstraction Layer)** — единый слой работы с оборудованием разных моделей Lumia
2. **Драйверы** — экран, сенсор, звук, камера, сеть, USB
3. **Файловая система**
4. **Менеджер процессов**
5. **Графический интерфейс**
6. **Системные приложения**
7. **Dima30 Store**
8. **Bolun AI**
9. **VPN Framework**
10. **Портирование на реальные Lumia**

### Overall Project Assessment

| Критерий | Оценка |
|----------|--------|
| 🏗️ Архитектура | 9.5/10 |
| 📁 Организация проекта | 10/10 |
| 💻 Кодовая база | 8/10 |
| 📱 Готовность к работе на устройстве | ~4/10 |

**Заключение**: Проект имеет хороший фундамент и правильную архитектуру. Основной объём работы — реализация функциональных подсистем, которые превратят архитектуру в полноценную операционную систему.

---

## Legal note

This repository does **not** contain Windows Phone 8.1 source code and does **not** claim binary/API compatibility with Windows Phone, Redmi firmware, Android, or proprietary Lumia firmware. User-[...]
