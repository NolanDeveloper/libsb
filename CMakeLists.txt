cmake_minimum_required(VERSION 3.2)

project(libsb)

include(CTest)
include(FetchContent)

set(CMAKE_C_STANDARD 11)

FetchContent_Declare(
        libgb
        GIT_REPOSITORY https://github.com/nolanrus/libgb.git
        GIT_TAG        bcebe9ef9ec50ffdd52df61dc9e3892e7ef4f1c9
)
FetchContent_Declare(
        libutf
        GIT_REPOSITORY https://github.com/nolanrus/libutf.git
        GIT_TAG        91fc9ed1debafd0dbd20d24b227d7c793d16da41
)
FetchContent_MakeAvailable(libgb libutf)

add_subdirectory(include)
add_subdirectory(src)

if(BUILD_TESTING)
    FetchContent_Declare(
            acutest
            GIT_REPOSITORY https://github.com/mity/acutest.git
            GIT_TAG        cce300734bfe5c3879b0449ac283a872633e615c
    )
    FetchContent_MakeAvailable(acutest)

    add_subdirectory(test-unit)
endif()