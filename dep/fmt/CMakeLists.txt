# Copyright (C) 2008-2019 TrinityCore <https://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
set(LIBRARY_NAME fmt)

include(CheckSymbolExists)
if (WIN32)
 check_symbol_exists(open io.h HAVE_OPEN)
else ()
 check_symbol_exists(open fcntl.h HAVE_OPEN)
endif ()

set(VERBOSE 1)

file(GLOB SRC_GRP_FMT *.cc *.h)
source_group("fmt" FILES ${SRC_GRP_FMT})

set(FMT_SOURCES
  ${SRC_GRP_FMT}
)

#if (HAVE_OPEN)
#  set(FMT_SOURCES ${FMT_SOURCES}
#   ${CMAKE_CURRENT_SOURCE_DIR}/posix.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/posix.cc)
# endif()

add_library(${LIBRARY_NAME} ${FMT_SOURCES})

 target_include_directories(${LIBRARY_NAME}
	PUBLIC
		${SRC_GRP_FMT}
 )

target_compile_definitions(${LIBRARY_NAME}
  PUBLIC
    -DFMT_USE_OVERRIDE
    -DFMT_USE_VARIADIC_TEMPLATES
    -DFMT_USE_RVALUE_REFERENCES
    -DFMT_USE_DELETED_FUNCTIONS
    -DFMT_USE_EXTERN_TEMPLATES)


if(WIN32)
    target_link_libraries(${LIBRARY_NAME})
endif()

