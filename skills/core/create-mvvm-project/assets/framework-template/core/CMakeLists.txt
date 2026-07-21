# MediatorCore - pure message framework core library
# Only depends on Qt5::Core, can be reused across projects independently
#
# Contains:
#   - mediator/    Mediator, Observe, Topic
#   - actor/       ActorObserve
#   - policy/      ISubscriptionPolicy, AlwaysPolicy, ValueChangedPolicy
#   - model/       IModel, BaseModel, LogEntry

add_library(MediatorCore
    # policy
    policy/ISubscriptionPolicy.h
    policy/AlwaysPolicy.h
    policy/ValueChangedPolicy.h

    # mediator
    mediator/Observe.h
    mediator/Observe.cpp
    mediator/Topic.h
    mediator/Topic.cpp
    mediator/Mediator.h
    mediator/Mediator.cpp

    # actor
    actor/ActorObserve.h
    actor/ActorObserve.cpp

    # viewmodel
    viewmodel/BaseViewModel.h

    # model
    model/BaseModel.h
    model/BaseModel.cpp
    model/LogEntry.h
    model/IModel.h
)

target_include_directories(MediatorCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/actor
        ${CMAKE_CURRENT_SOURCE_DIR}/mediator
        ${CMAKE_CURRENT_SOURCE_DIR}/model
        ${CMAKE_CURRENT_SOURCE_DIR}/policy
        ${CMAKE_CURRENT_SOURCE_DIR}/viewmodel
)

target_link_libraries(MediatorCore
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
)

target_compile_features(MediatorCore PUBLIC cxx_std_17)

add_library(MediatorCore::MediatorCore ALIAS MediatorCore)

# ========== install commands ==========

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/MediatorCore
    FILES_MATCHING PATTERN "*.h"
    PATTERN "CMakeLists.txt" EXCLUDE
    PATTERN "*.cpp" EXCLUDE
    PATTERN "build" EXCLUDE
)

install(TARGETS MediatorCore
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/MediatorCoreConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MediatorCoreConfig.cmake
    INSTALL_DESTINATION lib/cmake/MediatorCore
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/MediatorCoreConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MediatorCoreConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MediatorCoreConfigVersion.cmake
    DESTINATION lib/cmake/MediatorCore
)
