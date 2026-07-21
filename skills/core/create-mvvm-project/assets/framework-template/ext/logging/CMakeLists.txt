# MediatorLogging - log persistence + log list UI library
# Depends on MediatorCore + MediatorBinding + Qt5::Widgets + Qt5::Sql
#
# Purpose: Subscribe to log messages via Mediator, persist to SQLite, provide UI list display

add_library(MediatorLogging STATIC
    LogModel.h
    LogModel.cpp
    LogListViewModel.h
    LogListViewModel.cpp
    LogListView.h
    LogListView.cpp
    LogListView.ui
)

target_include_directories(MediatorLogging
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MediatorLogging
    PUBLIC
        MediatorCore
        MediatorBinding
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
)

target_compile_features(MediatorLogging PUBLIC cxx_std_17)
