cmake_minimum_required(VERSION 3.20)
project(BolunPhoneOS VERSION 1.0.0 LANGUAGES C CXX ASM_NASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

add_library(bolun_boot STATIC
    src/boot/asm/boot_stub.asm
    src/boot/asm/cpu_probe.asm
    src/boot/asm/interrupts.asm
)
set_target_properties(bolun_boot PROPERTIES NASM_OBJ_FORMAT elf64)
target_include_directories(bolun_boot PUBLIC include)

add_library(bolun_core STATIC
    src/kernel/c/bolun_kernel.c
    src/kernel/c/bolun_runtime.c
    src/kernel/cpp/app_manager.cpp
    src/kernel/cpp/bolun_core_bridge.cpp
    src/kernel/cpp/messaging_service.cpp
    src/kernel/cpp/notification_center.cpp
    src/kernel/cpp/people_hub.cpp
    src/kernel/cpp/tile_service.cpp
)
target_include_directories(bolun_core PUBLIC include src/kernel/cpp)
target_link_libraries(bolun_core PUBLIC bolun_boot)

add_executable(bolun_demo src/kernel/c/bolun_demo.c)
target_link_libraries(bolun_demo PRIVATE bolun_core)


add_library(bolun_phone10 STATIC
    boot/boot.c
    kernel/kernel.c
    hal/hal.c
    drivers/drivers.c
    mm/mm.c
    fs/fs.c
    net/net.c
    security/security.c
    ipc/ipc.c
    scheduler/scheduler.c
    power/power.c
    graphics/graphics.c
    display/display.c
    audio/audio.c
    camera/camera.c
    input/input.c
    storage/storage.c
    services/services.c
    framework/framework.c
    shell/shell.c
    sdk/sdk.c
    apps/apps.c
)
target_include_directories(bolun_phone10 PUBLIC
    boot
    kernel
    hal
    drivers
    mm
    fs
    net
    security
    ipc
    scheduler
    power
    graphics
    display
    audio
    camera
    input
    storage
    services
    framework
    shell
    sdk
    apps
    tests
)

add_executable(bolun_phone10_module_tests tests/bolun_phone10_module_tests.c)
target_link_libraries(bolun_phone10_module_tests PRIVATE bolun_phone10)
add_test(NAME bolun_phone10_module_tests COMMAND bolun_phone10_module_tests)

if(BUILD_TESTING)
    add_executable(bolun_core_tests tests/cpp/bolun_core_tests.cpp)
    target_link_libraries(bolun_core_tests PRIVATE bolun_core)
    add_test(NAME bolun_core_tests COMMAND bolun_core_tests)
endif()

add_subdirectory(bolun-phone-1.0)
