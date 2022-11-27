#*****************************************************
# specify minimum version of CMake to use
# (find_package requires 3.23)
#*****************************************************
cmake_minimum_required(VERSION 3.23)

#*****************************************************
project(SaturationChange LANGUAGES CXX)
#*****************************************************

add_subdirectory(App)
add_subdirectory(LibCpp)
add_subdirectory(LibASM)
