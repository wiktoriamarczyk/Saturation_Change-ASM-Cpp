#*****************************************************
# specify minimum version of CMake to use
# (find_package requires 3.23)
#*****************************************************
cmake_minimum_required(VERSION 3.23)

#*****************************************************
project(SaturationChange LANGUAGES CXX)
#*****************************************************

#*****************************************************
# add source files
#*****************************************************

SET(APP_SRC_FILES_LIST
	SaturationChange/stb_image.h
	SaturationChange/stb_image_write.h
	SaturationChange/Source.cpp
    SaturationChange/MainWindow.ui
    SaturationChange/MainWindow.h
    SaturationChange/MainWindow.cpp
	SaturationChange/Pixel.h
	SaturationChange/Pixel.cpp
)

#*****************************************************
# Find QT6 library
#*****************************************************
find_package(Qt6 REQUIRED COMPONENTS Widgets Core)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_program(WINDEPLOYQT_PATH NAMES windeployqt PATHS ${Qt6_DIR}\\bin)

#*****************************************************
# generate app exe
#*****************************************************

ADD_EXECUTABLE(SaturationChange ${APP_SRC_FILES_LIST} )

#*****************************************************
# enable multi processor compilation
#  - for visual "/MP" flag must be set
#*****************************************************

if(MSVC)
    target_compile_options(SaturationChange PRIVATE "/MP")
endif()

#*****************************************************
# Set language version to C++20
#*****************************************************

set_property(TARGET SaturationChange PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#*****************************************************
# Specify thst we want to link to Qt6::Widgets widgets
#*****************************************************

target_link_libraries(SaturationChange PRIVATE Qt6::Widgets)

#*****************************************************
# Do not create console for Windows Application
#*****************************************************

set_target_properties(SaturationChange PROPERTIES
    WIN32_EXECUTABLE ON
)

#*****************************************************
# add this to autoomaticly copy QT dlls to final directory
#*****************************************************

#add_custom_command(TARGET SaturationChange POST_BUILD
#    COMMAND ${WINDEPLOYQT_PATH} ARGS
#        --dir $<TARGET_FILE_DIR:SaturationChange>
#        --no-translations
#        --no-compiler-runtime
#        --no-system-d3d-compiler
#        --no-opengl-sw
#        --no-svg
#        $<TARGET_FILE:SaturationChange>
#)