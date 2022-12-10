add_executable(libsb_tests main.c)

target_link_libraries(libsb_tests
        PUBLIC libsb)

target_include_directories(libsb_tests
        PUBLIC ${acutest_SOURCE_DIR}/include)

add_test(unit libsb_tests)
