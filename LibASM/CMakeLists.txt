project(SaturationChange LANGUAGES)
#*****************************************************

#*****************************************************
# add source files
#*****************************************************

SET(LIB_SRC_FILES_LIST
	LibASM.asm
	LibASM.def
)

enable_language(ASM_MASM)

#*****************************************************
# generate dll
#*****************************************************

add_library(LibASM SHARED ${LIB_SRC_FILES_LIST} )
set_property(SOURCE LibASM.asm PROPERTY LANGUAGE ASM_MASM)

if(MSVC)
    target_link_options(LibASM PRIVATE "/NOENTRY")
endif()
