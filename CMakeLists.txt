cmake_minimum_required(VERSION 3.1)
include(FetchContent)

FetchContent_Declare(
    liburing_cmake
    GIT_REPOSITORY https://github.com/sharpeye/liburing-cmake.git
    GIT_TAG main
)
FetchContent_MakeAvailable(liburing_cmake)
add_executable(liburing_tcp
               src/main.cpp
               src/ioring/ioring.h src/ioring/ioring.cpp
               src/server/server.h src/server/server.cpp
               src/client/client.h src/client/client.cpp)

target_link_libraries(liburing_tcp PRIVATE liburing)
