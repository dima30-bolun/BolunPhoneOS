cmake_minimum_required(VERSION 3.20)
project(BolunPhone10CleanRoom VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(CTest)

add_library(bolun10_system STATIC
    system/core/bolun_system.c
    hardware/hal/bolun_hal.c
)
target_include_directories(bolun10_system PUBLIC system/core hardware/hal)
target_compile_options(bolun10_system PRIVATE -Wall -Wextra -Wpedantic)

add_executable(bolun10_cts cts/tests/bolun_system_tests.c)
target_link_libraries(bolun10_cts PRIVATE bolun10_system)
add_test(NAME bolun10_cts COMMAND bolun10_cts)


set(BOLUN10_IMAGE_DIR ${CMAKE_BINARY_DIR}/images)
add_custom_command(
    OUTPUT
        ${BOLUN10_IMAGE_DIR}/boot.img
        ${BOLUN10_IMAGE_DIR}/kernel.img
        ${BOLUN10_IMAGE_DIR}/mainos.img
        ${BOLUN10_IMAGE_DIR}/recovery.img
        ${BOLUN10_IMAGE_DIR}/sdk.tar
        ${BOLUN10_IMAGE_DIR}/docs.tar
        ${BOLUN10_IMAGE_DIR}/build.log
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BOLUN10_IMAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E echo "Bolun Boot image for ${PROJECT_VERSION}" > ${BOLUN10_IMAGE_DIR}/boot.img
    COMMAND ${CMAKE_COMMAND} -E echo "Bolun Kernel image for ${PROJECT_VERSION}" > ${BOLUN10_IMAGE_DIR}/kernel.img
    COMMAND ${CMAKE_COMMAND} -E echo "Bolun MainOS image for ${PROJECT_VERSION}" > ${BOLUN10_IMAGE_DIR}/mainos.img
    COMMAND ${CMAKE_COMMAND} -E echo "Bolun Recovery image for ${PROJECT_VERSION}" > ${BOLUN10_IMAGE_DIR}/recovery.img
    COMMAND ${CMAKE_COMMAND} -E tar cf ${BOLUN10_IMAGE_DIR}/sdk.tar --format=gnutar ${CMAKE_SOURCE_DIR}/sdk
    COMMAND ${CMAKE_COMMAND} -E tar cf ${BOLUN10_IMAGE_DIR}/docs.tar --format=gnutar ${CMAKE_SOURCE_DIR}/docs
    COMMAND ${CMAKE_COMMAND} -E echo "images generated" > ${BOLUN10_IMAGE_DIR}/build.log
    DEPENDS bolun10_cts
    VERBATIM
)
add_custom_target(bolun10_images ALL
    DEPENDS
        ${BOLUN10_IMAGE_DIR}/boot.img
        ${BOLUN10_IMAGE_DIR}/kernel.img
        ${BOLUN10_IMAGE_DIR}/mainos.img
        ${BOLUN10_IMAGE_DIR}/recovery.img
        ${BOLUN10_IMAGE_DIR}/sdk.tar
        ${BOLUN10_IMAGE_DIR}/docs.tar
        ${BOLUN10_IMAGE_DIR}/build.log
)
