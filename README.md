# BolunPhoneOS

BolunPhoneOS is an original, clean-room educational phone operating-system prototype branded as **Bolun Phone** by **dima30**. It uses the same broad product idea as classic tile-based phone systems—lock screen, live tiles, app list, notifications, contacts, messages, store, media, maps, settings—but the code, names, and assets are Bolun-owned placeholders rather than proprietary Microsoft Windows Phone source or artwork.

## What is included

- **C kernel API** in `src/kernel/c/` for boot, shutdown, app registry, launch, notifications, contacts, and messaging.
- **C++ service layer** in `src/kernel/cpp/` for native app, tile, notification, people, and messaging services.
- **Assembler HAL stubs** in `src/boot/asm/` for a boot banner, CPU probe flag, and interrupt placeholder.
- **C# shell prototype** in `src/ui/csharp/BolunPhone.Shell/` that renders a Bolun lock screen, tile start screen, and all-apps list.
- **CMake build** for the native C/C++/ASM core and tests.

## Architecture

```text
include/bolun/                 Public C ABI headers
src/boot/asm/                  Educational boot/HAL assembler stubs
src/kernel/c/                  Kernel lifecycle and C API
src/kernel/cpp/                Native service implementations
src/ui/csharp/BolunPhone.Shell C# lock screen, start screen, app list demo
tests/cpp/                     Native core tests
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
Bolun Phone home:
[CALL] Phone
[SMS] Messages
```

## Run C# shell prototype

If the .NET SDK is installed:

```bash
dotnet run --project src/ui/csharp/BolunPhone.Shell/BolunPhone.Shell.csproj
```

The C# shell is intentionally original: it does not use Microsoft Store, Office, Edge, Lumia, Nokia, or Windows icons. It renders Bolun equivalents such as **Bolun Store**, **Bolun Mail**, **Music**, **People**, **Weather**, **News**, and **Maps**.

## Legal note

This repository does **not** contain Windows Phone 8.1 source code and does **not** claim binary/API compatibility with Windows Phone. User-provided screenshots are treated only as visual references for an original Bolun-themed tile interface.
