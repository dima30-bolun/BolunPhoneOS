# Bolun Phone 1.0 project analysis and generated system map

## Analysis of the existing archive

The archive is a compact prototype with a native core, a managed shell demo, assembly placeholders, public C ABI headers, host tests, documentation, and utility tooling.  The dependency direction is intentionally simple: public headers define stable contracts, native C owns lifecycle and runtime state, C++ service code wraps higher-level managers, assembly code is linked as a low-level placeholder library, tests exercise the ABI, and the shell prototype consumes catalog concepts at a higher level.

## Original project map

| Path | Role | Main dependency direction |
| --- | --- | --- |
| `include/bolun/` | Public ABI for kernel, runtime, boot, result, and shared types | Used by native C, C++, demos, and tests |
| `src/boot/asm/` | Low-level boot and interrupt placeholders | Linked into the native core through the build system |
| `src/kernel/c/` | Kernel lifecycle, runtime tables, app catalog, and demo entry | Depends on public headers |
| `src/kernel/cpp/` | Service wrappers for apps, tiles, messaging, notifications, people, and bridge code | Depends on public headers and C++ service headers |
| `src/ui/csharp/` | Host-side shell demonstration | Separate UI prototype, not part of the native static core |
| `tests/cpp/` | Native smoke and integration tests | Links the native core |
| `tools/` | Host tools such as the MainOS development packer | Reads local payload files and emits deterministic images |
| `docs/` | Bring-up notes and architecture records | Human-facing design material |

## Existing subsystem observations

1. **Build system**: CMake coordinates C, C++, and NASM targets, then wires tests through CTest.
2. **Boot sequence**: host builds link placeholder boot objects first, then kernel/runtime code boots deterministic catalogs.
3. **Memory model**: fixed-size runtime tables model memory blocks, quotas, and usage totals without background allocation after runtime creation.
4. **Scheduler**: round-robin process selection advances across ready and running descriptors.
5. **Driver model**: HAL descriptors are registered by device class, ID, driver name, model, and power/presence flags.
6. **File model**: fixed records represent path, size, content hash, archive state, and trash state.
7. **Network model**: represented as catalog and permission contracts in the current prototype rather than a packet engine.
8. **IPC**: bounded queues carry sender, receiver, channel, body, and sequence values.
9. **Modules**: existing service modules are statically linked rather than dynamically loaded.
10. **Security**: permission bits and signed package records provide the current sandbox basis.
11. **Hardware initialization**: target profile selection installs HAL descriptors during runtime creation.
12. **Subsystem interfaces**: the project favors C ABI structs, fixed buffers, status codes, and host-side deterministic behavior.

## Generated clean-room Bolun Phone 1.0 tree

The new top-level tree adds independent subsystem directories requested for Bolun Phone 1.0.  Each directory contains a README, a public C header, and an original implementation stub.  The generated interfaces support ARMv7, ARM64, x86, and x86_64 by accepting a `BolunCpuFamily` value during initialization.

| Directory | Generated purpose |
| --- | --- |
| `boot/` | Own boot manifest validation and entry selection contracts |
| `kernel/` | Own global kernel state and startup orchestration contracts |
| `hal/` | Normalize board resources into portable channels |
| `drivers/` | Bind device classes to HAL channels |
| `mm/` | Track memory regions, virtual spaces, and quotas |
| `fs/` | Provide Bolun file-store object and mount contracts |
| `net/` | Model link, route, socket, and packet queue contracts |
| `security/` | Check signatures, permissions, and sandbox policy |
| `ipc/` | Move bounded messages between processes and services |
| `scheduler/` | Select runnable tasks across supported CPU families |
| `power/` | Coordinate sleep states, battery policy, and wake sources |
| `graphics/` | Accept surfaces and produce shell frames |
| `display/` | Map composited frames to display panels |
| `audio/` | Route capture and playback streams |
| `camera/` | Control sensor sessions and image buffers |
| `input/` | Normalize touch, buttons, and sensors |
| `storage/` | Enumerate block devices and expose volumes |
| `services/` | Host package, update, notification, and settings services |
| `framework/` | Expose stable Bolun APIs to applications |
| `shell/` | Render lock screen, live tiles, launcher, and notices |
| `sdk/` | Define BEN executable and BAPP package tooling contracts |
| `apps/` | Hold built-in Bolun applications |
| `build/` | Store image recipes |
| `configs/` | Store board, feature, and product policy files |
| `tests/` | Store cross-subsystem verification fixtures |

## Interface pattern

Every generated module exports four functions: `init`, `describe`, `health`, and `route`.  This creates a uniform boot-time contract while avoiding copied source.  The implementation is deliberately deterministic so the same code can be compiled for host validation now and replaced by deeper device-specific logic module by module later.
