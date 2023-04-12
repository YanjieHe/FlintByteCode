file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp)

file(GLOB TESTS
    ${PROJECT_SOURCE_DIR}/tests/*.cpp)

include_directories(
    ${PROJECT_SOURCE_DIR}/libs/
    ${PROJECT_SOURCE_DIR}/include/)

add_executable(FlintByteCodeTest
    ${SOURCES}
    ${TESTS})