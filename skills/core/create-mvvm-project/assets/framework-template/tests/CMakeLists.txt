add_executable(TopicPolicyTests
    TopicPolicyTests.cpp
)

target_link_libraries(TopicPolicyTests
    PRIVATE
        MediatorCore
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)

set(_test_qt_core_dir "${Qt${QT_VERSION_MAJOR}Core_DIR}")
get_filename_component(_test_qt_cmake_dir "${_test_qt_core_dir}" DIRECTORY)
get_filename_component(_test_qt_lib_dir "${_test_qt_cmake_dir}" DIRECTORY)
get_filename_component(_test_qt_root_dir "${_test_qt_lib_dir}" DIRECTORY)
set(_test_qt_bin_dir "${_test_qt_root_dir}/bin")

add_test(
    NAME TopicPolicyTests
    COMMAND ${CMAKE_COMMAND} -E env
        "PATH=${_test_qt_bin_dir}\\;$ENV{PATH}"
        $<TARGET_FILE:TopicPolicyTests>
)

add_executable(MediatorBenchmark
    MediatorBenchmark.cpp
)

target_link_libraries(MediatorBenchmark
    PRIVATE
        MediatorCore
        Qt${QT_VERSION_MAJOR}::Core
)

add_test(
    NAME MediatorBenchmark
    COMMAND ${CMAKE_COMMAND} -E env
        "PATH=${_test_qt_bin_dir}\\;$ENV{PATH}"
        $<TARGET_FILE:MediatorBenchmark>
)

if(WIN32 AND TARGET MediatorBenchmark)
    find_program(QT_DEPLOY_TOOLS
        NAMES windeployqt windeployqt.exe
        HINTS "${_test_qt_bin_dir}"
        PATHS ENV PATH
    )
    if(QT_DEPLOY_TOOLS)
        add_custom_command(TARGET MediatorBenchmark POST_BUILD
            COMMAND "${QT_DEPLOY_TOOLS}" "$<TARGET_FILE:MediatorBenchmark>"
                --dir "$<TARGET_FILE_DIR:MediatorBenchmark>"
                --no-translations --no-compiler-runtime --no-angle --no-opengl-sw
                $<$<CONFIG:Debug>:--debug>
                $<$<NOT:$<CONFIG:Debug>>:--release>
            COMMENT "Deploying Qt dependencies for MediatorBenchmark..."
        )
    endif()
endif()

add_executable(ModbusModuleTests
    ModbusModuleTests.cpp
)

target_link_libraries(ModbusModuleTests
    PRIVATE
        MediatorModbus
        Qt${QT_VERSION_MAJOR}::Test
)

add_test(
    NAME ModbusModuleTests
    COMMAND ${CMAKE_COMMAND} -E env
        "PATH=${_test_qt_bin_dir}\\;$ENV{PATH}"
        $<TARGET_FILE:ModbusModuleTests>
)

add_executable(SinewaveDemoTests
    SinewaveDemoTests.cpp
    ${CMAKE_SOURCE_DIR}/demo/sinewave/SineModel.cpp
    ${CMAKE_SOURCE_DIR}/app/log/Logger.cpp
)

target_include_directories(SinewaveDemoTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/demo/sinewave
        ${CMAKE_SOURCE_DIR}/app/log
)

target_link_libraries(SinewaveDemoTests
    PRIVATE
        MediatorCore
        MediatorLogging
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)

add_test(
    NAME SinewaveDemoTests
    COMMAND ${CMAKE_COMMAND} -E env
        "PATH=${_test_qt_bin_dir}\\;$ENV{PATH}"
        $<TARGET_FILE:SinewaveDemoTests>
)
