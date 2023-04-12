file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp)

include_directories(
    ${PROJECT_SOURCE_DIR}/libs/
    ${PROJECT_SOURCE_DIR}/include/)

add_executable(FlintByteCode
    ${PROJECT_SOURCE_DIR}/Main.cpp
    ${SOURCES})