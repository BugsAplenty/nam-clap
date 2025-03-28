cmake_minimum_required(VERSION 3.15)
project(MyNamClap LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Pull in CLAP
set(CLAP_BUILD_TESTS OFF CACHE BOOL "Disable CLAP tests")

add_subdirectory(extern/clap)
add_subdirectory(extern/NeuralAmpModelerCore)

add_library(my_nam_clap SHARED
    src/main.cpp
    src/plugin.cpp
)

target_link_libraries(my_nam_clap PRIVATE clap NeuralAmpModelerCore)
# Now you can do #include <clap/clap.h> in your code

