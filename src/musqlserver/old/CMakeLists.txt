#/**
# * MuMySQLServer is a full featured server for World of Warcraft, supporting
# * the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
# *
# * Copyright (C) 2005-2019  MuMySQLServer project <http://getmangos.eu>
# *
# * This program is free software; you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation; either version 2 of the License, or
# * (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program; if not, write to the Free Software
# * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# *
# * World of Warcraft, and all World of Warcraft or Warcraft art, images,
# * and lore are copyrighted by Blizzard Entertainment, Inc.
# */

set (CMAKE_SYSTEM_VERSION 8.1 CACHE TYPE INTERNAL FORCE)

set(EXECUTABLE_NAME MuOnline)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

#MuMySQL Server Files
file(GLOB_RECURSE SRC_GRP_MUSHARED *.cpp *.h)
source_group("Source Files" FILES ${SRC_GRP_MUSHARED})

ADD_SUBDIRECTORY("Custom")
ADD_SUBDIRECTORY("Eventos")

set(EXECUTABLE_SRCS
    ${SRC_GRP_MUSHARED}
)

if(WIN32)
    include(generate_product_version)
    generate_product_version(
        VersionFilesOutputVariable
        NAME "muShared.exe"
        ICON ${CMAKE_SOURCE_DIR}/src/musqlserver/realmd.ico
        VERSION_MAJOR 0
        VERSION_MINOR 1
        VERSION_PATH ${BUILD_COUNTER}
        VERSION_REVISION ${BUILD_REVISION}
        COMPANY_NAME "getmangos.eu"
        # Do not alter, remove or prevent COMPANY_COPYRIGHT from displaying
        COMPANY_COPYRIGHT
        FILE_DESCRIPTION "Mu Server"
    )
    # add resource file to windows build
    set(EXECUTABLE_SRCS ${EXECUTABLE_SRCS})
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
	${SRC_GRP_MUSHARED}
)

add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SRCS})

target_link_libraries(${EXECUTABLE_NAME}
)

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} ${BOOST_LIBRARY})



set(EXECUTABLE_LINK_FLAGS "")

if(UNIX)
    if (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        set(EXECUTABLE_LINK_FLAGS "-Wl,--no-as-needed -pthread -lrt ${EXECUTABLE_LINK_FLAGS}")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(EXECUTABLE_LINK_FLAGS "-Wl,--no-as-needed -ldl -pthread -lrt ${EXECUTABLE_LINK_FLAGS}")
    endif()
endif()

if(APPLE)
    set(EXECUTABLE_LINK_FLAGS "-framework Carbon ${EXECUTABLE_LINK_FLAGS}")
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINK_FLAGS "${EXECUTABLE_LINK_FLAGS}")

install(TARGETS ${EXECUTABLE_NAME} DESTINATION "${BIN_DIR}")

if(WIN32 AND MSVC)
    install(FILES $<TARGET_PDB_FILE:${EXECUTABLE_NAME}> DESTINATION ${BIN_DIR} OPTIONAL)
endif()
