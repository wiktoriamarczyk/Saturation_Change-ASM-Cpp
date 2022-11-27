project(SaturationChange LANGUAGES CXX)
#*****************************************************

#*****************************************************
# add source files
#*****************************************************

SET(LIB_SRC_FILES_LIST
    SaturationChangeCpp.h
	SaturationChangeCpp.cpp
	SaturationChangeCpp.def
)

#*****************************************************
# generate dll
#*****************************************************

add_library(LibCpp SHARED ${LIB_SRC_FILES_LIST} )

#*****************************************************
# enable multi processor compilation
#  - for visual "/MP" flag must be set
#*****************************************************

if(MSVC)
    target_compile_options(LibCpp PRIVATE "/MP")
endif()