# demo/sinewave - Sine Wave Demo
# Dependencies: MediatorCore + MediatorBinding + MediatorLogging + MediatorRuntime + Qt5::Charts

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Charts)

add_executable(SinewaveDemo
    SineModel.h
    SineModel.cpp
    SineViewModel.h
    SineViewModel.cpp
    SineView.h
    SineView.cpp
    main.cpp
)

target_include_directories(SinewaveDemo
    PRIVATE
        ${CMAKE_SOURCE_DIR}/core
        ${CMAKE_SOURCE_DIR}/core/actor
        ${CMAKE_SOURCE_DIR}/core/mediator
        ${CMAKE_SOURCE_DIR}/core/model
        ${CMAKE_SOURCE_DIR}/core/policy
        ${CMAKE_SOURCE_DIR}/core/viewmodel
        ${CMAKE_SOURCE_DIR}/ext/binding
        ${CMAKE_SOURCE_DIR}/ext/logging
        ${CMAKE_SOURCE_DIR}/app
        ${CMAKE_SOURCE_DIR}/app/log
)

target_link_libraries(SinewaveDemo
    PRIVATE
        MediatorCore
        MediatorBinding
        MediatorLogging
        MediatorRuntime
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Charts
        Qt${QT_VERSION_MAJOR}::Sql
)

target_compile_features(SinewaveDemo PRIVATE cxx_std_17)
