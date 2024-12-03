cmake_minimum_required(VERSION 3.20.0)

# Verify that the environment is correctly set up.
if (DEFINED ENV{PICO_SDK_PATH})
  include("$ENV{PICO_SDK_PATH}/pico_sdk_init.cmake")
else()
  message(FATAL_ERROR "PICO_SDK_PATH environment variable not set")
endif()

if (NOT DEFINED ENV{REACTOR_UC_PATH})
  message(FATAL_ERROR "REACTOR_UC_PATH environment variable not set. source env.bash in reactor-uc")
endif()

project(reactor-uc-pico)
pico_sdk_init()

set(LF_APP AccelerometerDisplay CACHE STRING "Name of the LF application")
set(PLATFORM "PICO" CACHE STRING "Platform to target")

# Run LFC to generate C code when any LF file under `src` changes.
message(STATUS "Executing $ENV{REACTOR_UC_PATH}/lfc/bin/lfc-dev -c src/${LF_APP}.lf")
file(GLOB_RECURSE LF_SRCS CONFIGURE_DEPENDS *.lf)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${LF_SRCS})
execute_process(
  COMMAND $ENV{REACTOR_UC_PATH}/lfc/bin/lfc-dev src/${LF_APP}.lf
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND_ERROR_IS_FATAL ANY
)

# Build the generated C code.
include(src-gen/${LF_APP}/CMakeLists.txt)
add_subdirectory(${REACTOR_UC_PATH})
add_executable(${LF_APP} main.c ${LFC_GEN_SOURCES})
target_include_directories(${LF_APP} PRIVATE ${LFC_GEN_INCLUDE_DIRS})
target_link_libraries(${LF_APP} PRIVATE reactor-uc)

# Link with the polulu_3pi_2040_robot library.
add_subdirectory(robot-lib)
target_link_libraries(${LF_APP} PRIVATE robot)
target_include_directories(${LF_APP} PRIVATE robot-lib/src)