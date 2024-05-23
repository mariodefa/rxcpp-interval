cmake_minimum_required(VERSION 3.14)
project(myproject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add vcpkg.cmake
set(CMAKE_TOOLCHAIN_FILE "/vcpkg/scripts/buildsystems/vcpkg.cmake")

# add rxcpp external library
find_package(rxcpp CONFIG REQUIRED)

# Example1 
add_executable(Example1 example1.cpp)
target_link_libraries(Example1 PRIVATE rxcpp)
target_include_directories(Example1 PRIVATE /vcpkg/installed/x64-linux/include)