cmake_minimum_required(VERSION 3.10.0)
cmake_policy(SET CMP0091 NEW)

project(CMAKE_PROJECT_TEST VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(TestStaticLibrary)
add_subdirectory(TestDynamicLibrary)
add_subdirectory(TestExecutable)