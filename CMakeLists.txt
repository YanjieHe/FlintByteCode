cmake_minimum_required (VERSION 3.8)

project(FlintByteCode)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

# Only build executable and tests when this is the top-level project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_executable(FlintByteCode_exe Main.cpp)
    target_link_libraries(FlintByteCode_exe PRIVATE flint_bytecode)

    enable_testing()
    add_subdirectory(tests)
endif()