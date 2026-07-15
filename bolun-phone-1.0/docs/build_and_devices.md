# Building device images

Bolun Build reads `device/*/*/board.conf`, `kernel.config`, and `build.mk`, selects common platform sources, then applies BSP-specific HAL bindings. First-version targets are `lumia710`, `lumia625`, `lumia640lte`, `redmi9a`, and `redmi9c_nfc`.

```sh
cmake -S bolun-phone-1.0 -B build/bolun10
cmake --build build/bolun10
ctest --test-dir build/bolun10 --output-on-failure
```

Image creation then combines bootable manifests, kernel profile, BFS root, system services, Bolun UI, and built-in BAPP packages.
