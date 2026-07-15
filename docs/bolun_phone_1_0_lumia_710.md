# Bolun Phone 1.0 Lumia 710 bring-up plan

This repository is maintained as a clean-room Bolun prototype.  It does not import vendor phone firmware or third-party mobile platform source trees; instead it records the contracts that must be implemented before a real Lumia 710 MainOS image can be produced.

## Target profile

| Field | Value |
| --- | --- |
| Bolun codename | `lumia-710` |
| CPU family | ARMv7 / MSM7227A-class bring-up target |
| Boot path | Existing device boot chain loads a MainOS payload; Bolun replaces the MainOS contents only |
| Secure boot policy | Preserve the existing boot chain and hand off to signed MainOS content |
| Initial memory budget | 512 MiB profile, with fixed-capacity host-side runtime tables |

## Runtime status in this tree

The host runtime now exposes `lumia-710` as a first-class HAL profile.  The profile registers display, touch, GPIO buttons, haptics, audio, camera, flash, USB, Wi-Fi, Bluetooth, GPS, cellular/SIM, battery, charging, storage-card, and sensor contracts through the Bolun HAL registry.  The current implementation is a deterministic simulator suitable for tests and tooling; it is not yet a physical device driver stack.

## MainOS image contract

A Bolun MainOS image is represented by a small manifest followed by component payloads:

1. `BOLUNMAINOS` magic and format version.
2. Target codename, initially `lumia-710`.
3. Component table containing boot stub, kernel, runtime, framework, shell, system apps, and documentation payloads.
4. Per-component size and 32-bit integrity value for deterministic smoke verification.

The `tools/mkmainos.py` utility builds this development image format from local payloads.  It intentionally does not patch bootloaders, disable Secure Boot, or modify partitions.

## Implementation phases

1. **Boot hand-off**: replace the placeholder assembly banner with an ARMv7 entry path that validates the Bolun MainOS manifest and jumps to the kernel entry point.
2. **Bolun kernel**: graduate the existing process scheduler, memory accounting, IPC, timers, interrupt counters, and syscall model from host-side runtime tables into target code.
3. **Lumia 710 HAL**: bind each HAL descriptor to verified MSM7227A board resources, starting with serial logging, framebuffer scan-out, touch input, buttons, battery, and USB.
4. **System services**: move package, notification, theme, network, security, telephony, audio, and update managers behind stable Bolun APIs.
5. **Shell and apps**: replace console demos with a framebuffer shell that renders lock screen, live tiles, app list, notifications, quick settings, and built-in apps.
6. **Signing and sandboxing**: enforce package signatures, permissions, and per-process resource budgets before enabling third-party packages.

## Non-goals for this commit

- No imported vendor firmware or third-party mobile OS source.
- No bootloader replacement.
- No physical register writes from host tests.
- No claim that the current repository can be flashed to retail hardware.
