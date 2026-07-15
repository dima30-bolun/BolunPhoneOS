# hal/

## Purpose

Hardware abstraction for Bolun Phone 1.0. This directory is part of the clean-room implementation tree and contains original interfaces, stubs, and deterministic host-side logic.

## Responsibilities

- Normalizes board resources for portable services.
- Publish a narrow C interface that other Bolun subsystems can use without depending on private state.
- Keep initialization deterministic for host tests and future bare-device bring-up.

## Public API

- `bolun_hal_init` prepares module state for a selected CPU family.
- `bolun_hal_describe` returns a short static capability description.
- `bolun_hal_health` reports whether the module is initialized and ready.
- `bolun_hal_route` accepts a bounded command string and returns a deterministic status code.

## Interfaces

Inputs are plain structures and fixed-size text buffers. Outputs are numeric status values and immutable strings so these modules can be reused by boot code, kernel code, user services, and tests.
