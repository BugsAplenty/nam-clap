# src/CMakeLists.txt

# Compiler warnings / optimizations
if (MSVC)
    add_compile_options(
        "$<$<CONFIG:DEBUG>:/W4>"
        "$<$<CONFIG:RELEASE>:/O2>"
    )
else()
    add_compile_options(
        -Wall
        "$<$<CONFIG:DEBUG>:-Og;-ggdb>"
        "$<$<CONFIG:RELWITHDEBINFO>:-Ofast>"
        "$<$<CONFIG:RELEASE>:-Ofast>"
    )
endif()

# CPU optimizations
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(amd64)|(AMD64)|(x86_64)")
    option(USE_NATIVE_ARCH "Enable architecture-specific optimizations" OFF)

    if (USE_NATIVE_ARCH)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            add_compile_options(/arch:AVX2)
            message(STATUS "Enabling /arch:AVX2")
        else()
            add_compile_options(-march=x86-64-v3)
            message(STATUS "Enabling -march=x86-64-v3")
        endif()
    endif()
endif()

# Add NeuralAudio submodule
add_subdirectory(${CMAKE_SOURCE_DIR}/extern/NeuralAudio NeuralAudio)

# Plugin sources
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/plugin.cpp
    ${CMAKE_SOURCE_DIR}/src/dsp.cpp
)

# Optional: if you want to include headers in source groups (not necessary but cleaner IDE experience)
set(HEADERS
    ${CMAKE_SOURCE_DIR}/src/plugin.h
)

# Add library target
add_library(nam_clap MODULE ${SOURCES} ${HEADERS})

# Includes
target_include_directories(nam_clap PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/extern/clap/include
    ${CMAKE_SOURCE_DIR}/extern/NeuralAudio
)

# Link with NeuralAudio
target_link_libraries(nam_clap PRIVATE NeuralAudio)

# Properties
set_target_properties(nam_clap PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    PREFIX ""
    SUFFIX ".clap"
    OUTPUT_NAME "nam_clap"
)

# Platform specifics
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(nam_clap PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

# Denormals handling
option(DISABLE_DENORMALS "Disable floating point denormals" ON)

if (DISABLE_DENORMALS)
    target_compile_definitions(nam_clap PRIVATE DISABLE_DENORMALS)
endif()
