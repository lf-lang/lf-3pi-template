cmake_minimum_required(VERSION 3.20.0)

set(LF_MAIN_NAME "" CACHE STRING "Name of the LF application")
set(LF_SRC_GEN_PATH "" CACHE STRING "Name of the LF application")
set(PLATFORM "PICO" CACHE STRING "Platform to target")

# Verify that the environment is correctly set up.
if (DEFINED ENV{PICO_SDK_PATH})
  include("$ENV{PICO_SDK_PATH}/pico_sdk_init.cmake")
else()
  message(FATAL_ERROR "PICO_SDK_PATH environment variable not set")
endif()

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
project(reactor-uc-pico)

pico_sdk_init()

# Build the generated C code.
include(${LF_SRC_GEN_PATH}/${LF_MAIN_NAME}/CMakeLists.txt)
add_subdirectory(${REACTOR_UC_PATH})
add_executable(${LF_MAIN_NAME} ${LFC_GEN_MAIN} ${LFC_GEN_SOURCES})
target_include_directories(${LF_MAIN_NAME} PRIVATE ${LFC_GEN_INCLUDE_DIRS})
target_link_libraries(${LF_MAIN_NAME} PRIVATE reactor-uc)
target_link_libraries(${LF_MAIN_NAME} PUBLIC pico_stdlib pico_sync)

pico_enable_stdio_usb(${LF_MAIN_NAME} 1)
pico_enable_stdio_uart(${LF_MAIN_NAME} 1)

# Link with the polulu_3pi_2040_robot library.
add_subdirectory(robot-lib)
target_link_libraries(${LF_MAIN} PRIVATE robot)
target_include_directories(${LF_MAIN} PRIVATE robot-lib/src)
