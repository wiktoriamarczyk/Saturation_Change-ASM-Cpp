project(SaturationChange LANGUAGES CXX)

#*****************************************************
# add source files
#*****************************************************

SET(APP_SRC_FILES_LIST
    MainWindow.ui
    MainWindow.h
    MainWindow.cpp
	Pixel.h
	Pixel.cpp
	ScopeExit.h
	Source.cpp
	stb_image.h
	stb_image_write.h
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

ADD_EXECUTABLE(App ${APP_SRC_FILES_LIST} )

#*****************************************************
# enable multi processor compilation
#  - for visual "/MP" flag must be set
#*****************************************************

if(MSVC)
    target_compile_options(App PRIVATE "/MP")
endif()

#*****************************************************
# Set language version to C++20
#*****************************************************

set_property(TARGET App PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#*****************************************************
# Specify thst we want to link to Qt6::Widgets widgets
#*****************************************************

target_link_libraries(App PRIVATE Qt6::Widgets)
#target_link_libraries(App PRIVATE Lib)

#*****************************************************
# Do not create console for Windows Application
#*****************************************************

set_target_properties(App PROPERTIES
    WIN32_EXECUTABLE ON
)

#*****************************************************
# add this to autoomaticly copy QT dlls to final directory
#*****************************************************

add_custom_command(TARGET App POST_BUILD
    COMMAND ${WINDEPLOYQT_PATH} ARGS
        --dir $<TARGET_FILE_DIR:App>
        --no-translations
        --no-compiler-runtime
        --no-system-d3d-compiler
        #$<$<VERSION_LESS:${QT_VERSION},6>:--no-angle>
        --no-opengl-sw
        --no-svg
        #--no-libraries
        $<TARGET_FILE:App>
)