file(GLOB TESTS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(FlintByteCodeTest ${TESTS})

target_link_libraries(FlintByteCodeTest PRIVATE flint_bytecode)

target_include_directories(FlintByteCodeTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/libs)