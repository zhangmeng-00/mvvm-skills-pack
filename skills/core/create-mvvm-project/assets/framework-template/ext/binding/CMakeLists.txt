# MediatorBinding - UI property binding library
# Depends on MediatorCore + Qt5::Widgets
#
# Purpose: ViewModel Q_PROPERTY -> View Q_PROPERTY one-way binding

add_library(MediatorBinding STATIC
    Binding.h
    BindingHelper.h
    binding_autogen.cpp
)

target_include_directories(MediatorBinding
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MediatorBinding
    PUBLIC
        MediatorCore
        Qt${QT_VERSION_MAJOR}::Widgets
)

target_compile_features(MediatorBinding PUBLIC cxx_std_17)
