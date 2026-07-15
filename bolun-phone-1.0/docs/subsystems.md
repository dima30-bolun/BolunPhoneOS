# Bolun Phone 1.0 implemented host subsystems

The current clean-room host core now exercises concrete behavior instead of directory-only templates.

## Boot

`bolun10_boot_prepare` records target, image version, component count, and deterministic integrity value for image packaging.

## Kernel process and scheduler services

`bolun10_process_spawn` creates bounded process records with permissions, priority, memory budget, and state. `bolun10_process_schedule` performs deterministic round-robin scheduling and records CPU ticks.

## Memory and virtual memory

`bolun10_memory_alloc` accounts memory against per-process budgets. `bolun10_memory_map` binds a block to a virtual address and writable flag. `bolun10_memory_free` returns the budget.

## BFS file system

`bolun10_bfs_create` stores path, owner, size, directory bit, and checksum. `bolun10_bfs_read` returns file metadata for services and apps.

## BAPP package system

`bolun10_bapp_install` parses manifest keys, records installed packages, and derives permission bits from manifest records.

## IPC

`bolun10_ipc_send` and `bolun10_ipc_receive` implement a bounded message queue with sender, receiver, channel, payload, and sequence.

## Driver model and HAL

`bolun10_driver_register` and `bolun10_driver_power` track loaded/powered platform drivers. `bolun_hal_validate` enforces a complete HAL operation table.

## Network

`bolun10_socket_open` checks network permission and records local/remote endpoints. `bolun10_socket_send` tracks transmitted and synthetic response packets.

## Graphics, windows, and compositor

`bolun10_window_create` records windows owned by processes. `bolun10_compositor_frame` summarizes the visible frame set and power mode for Bolun UI.

## Power, update, recovery, diagnostics

`bolun10_power_set_low_power` toggles low-power mode. `bolun10_update_apply` validates an update signature before increasing generation. `bolun10_recovery_plan` writes a BFS recovery plan. `bolun10_diagnostics_report` produces a compact system report.

## Image generation

The standalone CMake project creates boot, kernel, MainOS, recovery, SDK, documentation, and build log artifacts under the build image directory.
