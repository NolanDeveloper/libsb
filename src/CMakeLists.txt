add_library(libsb libsb.c)
target_link_libraries(libsb
        PUBLIC libsb_interface
        PUBLIC libgb
        PUBLIC libutf)
