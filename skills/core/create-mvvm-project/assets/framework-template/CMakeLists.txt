cmake_minimum_required(VERSION 3.16)

# Architecture hint only. The actual platform is selected by the CMake preset.
set(ARCH "x86" CACHE STRING "Target architecture: x64 or x86")
set_property(CACHE ARCH PROPERTY STRINGS "x64" "x86")

project(MediatorMVAQt LANGUAGES CXX)

if(MSVC)
    if(NOT ARCH STREQUAL "x86" AND NOT ARCH STREQUAL "x64")
        message(FATAL_ERROR "ARCH must be x86 or x64, got '${ARCH}'")
    endif()

    if((ARCH STREQUAL "x86" AND CMAKE_SIZEOF_VOID_P EQUAL 8) OR
       (ARCH STREQUAL "x64" AND CMAKE_SIZEOF_VOID_P EQUAL 4))
        message(FATAL_ERROR
            "ARCH='${ARCH}' does not match the generated platform '${CMAKE_VS_PLATFORM_NAME}'. "
            "Reconfigure with the matching CMake preset.")
    endif()

    string(REPLACE "\\" "/" _cmake_prefix_path "${CMAKE_PREFIX_PATH}")
    if(ARCH STREQUAL "x86" AND _cmake_prefix_path MATCHES "msvc2019_64")
        message(FATAL_ERROR
            "ARCH='x86' cannot use the 64-bit Qt path '${CMAKE_PREFIX_PATH}'. "
            "Use the 'vs2019-x86' preset.")
    elseif(ARCH STREQUAL "x64" AND NOT _cmake_prefix_path MATCHES "msvc2019_64")
        message(FATAL_ERROR
            "ARCH='x64' expects the 64-bit Qt path in CMAKE_PREFIX_PATH. "
            "Current value: '${CMAKE_PREFIX_PATH}'. Use the 'vs2019-x64' preset.")
    endif()
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_VERSION_MAJOR 5)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql Network SerialBus)

if(MSVC)
    add_compile_options(/utf-8)
endif()

option(MEDIATOR_BUILD_TESTS "Build Mediator tests" OFF)

# ============================================================
# Framework libraries
# ============================================================

add_subdirectory(core)       # MediatorCore - pure message framework
add_subdirectory(app)        # MediatorRuntime - application runtime
add_subdirectory(ext)        # MediatorBinding, MediatorLogging
add_subdirectory(demo)       # Demo programs

# MediatorModbus - Modbus communication library (reference implementation)
add_library(MediatorModbus STATIC
    model/modbus/ModbusMasterModel.cpp
    model/modbus/ModbusMasterModel.h
    model/modbus/ModbusModuleBase.cpp
    model/modbus/ModbusModuleBase.h
    model/modbus/PidPressureModule.cpp
    model/modbus/PidPressureModule.h
    model/modbus/PointDef.h
)

target_include_directories(MediatorModbus
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/model/modbus
)

target_link_libraries(MediatorModbus
    PUBLIC
        MediatorCore
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::SerialBus
)

# ============================================================
# Tests
# ============================================================

if(MEDIATOR_BUILD_TESTS)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)
    enable_testing()
    add_subdirectory(tests)
endif()
