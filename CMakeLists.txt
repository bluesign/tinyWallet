# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(TinyWallet VERSION 1.2.0)


set(CMAKE_CXX_FLAGS "-fpermissive")
