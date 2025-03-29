cmake_minimum_required(VERSION 3.17)
project(MyNamClap VERSION 0.1.0)

# Add CLAP headers
add_subdirectory(extern/clap EXCLUDE_FROM_ALL)

# Add NeuralAudio
add_subdirectory(extern/NeuralAudio EXCLUDE_FROM_ALL)

#-----------------------------------------
# Global settings
#-----------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings / optional flags
if (CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
    add_compile_options(
        -fvisibility=hidden
        -fvisibility-inlines-hidden
        -Werror=return-type
        -Wno-unused-parameter
        -Wuninitialized
        -Woverloaded-virtual
    )
endif()

# OS-specific checks
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # on macOS
    include_directories(SYSTEM /usr/local/include)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # on Linux
    link_libraries(stdc++fs)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # on Windows
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
else()
    message(FATAL_ERROR "Unrecognized platform!")
endif()

#-----------------------------------------
# Where the plugin will install
#-----------------------------------------
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "The library install dir (default: lib)")

#-----------------------------------------
# Add subdirectory if you have external libs
# e.g. NeuralAudio or other submodules
#-----------------------------------------

#-----------------------------------------
# Build the plugin library
#-----------------------------------------
set(PLUGIN_SOURCES
    src/main.cpp
    src/plugin.cpp
    src/dsp.cpp
)

set(PLUGIN_NAME "nam_clap")
set(PLUGIN_FULL_NAME "${PLUGIN_NAME}.clap")

add_library(${PLUGIN_NAME} MODULE ${PLUGIN_SOURCES})

target_include_directories(${PLUGIN_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/clap/include
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/NeuralAudio  # Add NeuralAudio include path
)

target_link_libraries(${PLUGIN_NAME}
    PRIVATE
        NeuralAudio
)

# Ensure proper symbol visibility
set_target_properties(${PLUGIN_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    PREFIX ""
    SUFFIX ".clap"
    OUTPUT_NAME "${PLUGIN_NAME}"
)

#-----------------------------------------
# Define installation paths based on platform
#-----------------------------------------
if(UNIX AND NOT APPLE)
    set(CLAP_INSTALL_DIR "$ENV{HOME}/.clap" CACHE PATH "Default CLAP install location for Linux")
elseif(APPLE)
    set(CLAP_INSTALL_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/CLAP" CACHE PATH "Default CLAP install location for macOS")
elseif(WIN32)
    set(CLAP_INSTALL_DIR "$ENV{COMMONPROGRAMFILES}/CLAP" CACHE PATH "Default CLAP install location for Windows")
endif()

#-----------------------------------------
# Install the plugin
#-----------------------------------------
install(TARGETS ${PLUGIN_NAME}
    LIBRARY DESTINATION ${CLAP_INSTALL_DIR}
)

#-----------------------------------------
# (Optional) Build packaging with CPack
#-----------------------------------------
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "you@example.com")
set(CPACK_PACKAGE_VENDOR "Your Company")
set(CPACK_PACKAGE_DESCRIPTION "Neural Amp Modeler CLAP plugin")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NAM-based CLAP plugin")
set(CPACK_DEBIAN_PACKAGE_SECTION sound)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)
set(CPACK_PACKAGING_INSTALL_PREFIX /usr)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_STRIP_FILES ON)

include(CPack)
