# MediatorRuntime - application runtime infrastructure
# Contains: AppContext (Mediator thread management), Logger (structured logging)
#
# Depends on MediatorCore + Qt5::Core

add_library(MediatorRuntime STATIC
    AppContext.h
    AppContext.cpp
    log/Logger.h
    log/Logger.cpp
)

target_include_directories(MediatorRuntime
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MediatorRuntime
    PUBLIC
        MediatorCore
        Qt${QT_VERSION_MAJOR}::Core
)

target_compile_features(MediatorRuntime PUBLIC cxx_std_17)
