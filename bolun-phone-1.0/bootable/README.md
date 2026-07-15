# bootable/

## Purpose

Bootloader, recovery flow, early initialization manifests, and image hand-off contracts.

## Interfaces

This directory participates in the Bolun Phone 1.0 clean-room platform. Public interfaces are described in local headers, configuration files, and the subsystem documents under `docs/`. Components communicate through Bolun-defined C APIs, manifest records, BEN executable metadata, BAPP package metadata, and BSP configuration files.

## Build and test role

Files in this directory are designed to be consumed by Bolun Build. Host tests validate syntax, API shape, and deterministic behavior before device-specific code is enabled.
