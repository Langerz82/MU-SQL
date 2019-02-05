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

set(EXECUTABLE_NAME muMySQLServer)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

file(GLOB_RECURSE _source_list *.cpp* *.h* *.hpp*)
foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/src/musqlserver" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

set(EXECUTABLE_SRCS
    ${_source_list}
    Main.cpp
)

if(WIN32)
    include(generate_product_version)
    generate_product_version(
        VersionFilesOutputVariable
        NAME "muMySQLServer.exe"
        ICON ${CMAKE_SOURCE_DIR}/src/musqlserver/realmd.ico
        VERSION_MAJOR 0
        VERSION_MINOR 21
        VERSION_PATH ${BUILD_COUNTER}
        VERSION_REVISION ${BUILD_REVISION}
        COMPANY_NAME "getmangos.eu"
        # Do not alter, remove or prevent COMPANY_COPYRIGHT from displaying
        COMPANY_COPYRIGHT
        FILE_DESCRIPTION "Mu MySQL Server Daemon"
    )
    # add resource file to windows build
    set(EXECUTABLE_SRCS ${EXECUTABLE_SRCS} VersionResource.rc)
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/shared
    ${CMAKE_SOURCE_DIR}/src/shared/Common
    ${CMAKE_SOURCE_DIR}/src/shared/Utilities
    ${CMAKE_SOURCE_DIR}/src/shared/Log
    ${CMAKE_SOURCE_DIR}/src/shared/DataStores
    ${CMAKE_SOURCE_DIR}/src/shared/Win
    ${CMAKE_SOURCE_DIR}/src/shared/Linux
    ${CMAKE_SOURCE_DIR}/src/framework
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src/shared
    ${ACE_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/src/musqlserver
	${CMAKE_SOURCE_DIR}/src/musqlserver/old
	${CMAKE_SOURCE_DIR}/../muServerDeps/_Include
)

add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SRCS})

target_link_libraries(${EXECUTABLE_NAME}
    shared
    framework
    ${OPENSSL_LIBRARIES}
)

if(NOT ACE_USE_EXTERNAL)
    add_dependencies(${EXECUTABLE_NAME} ace)
    target_link_libraries(${EXECUTABLE_NAME} ace)
else()
    target_link_libraries(${EXECUTABLE_NAME} ACE)
endif()

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} ${BOOST_LIBRARY})

if(POSTGRESQL)
    include_directories(${PostgreSQL_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${PostgreSQL_LIBRARIES})
else()
    include_directories(${MYSQL_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${MYSQL_LIBRARY})
endif()

if(UNIX)
    target_link_libraries(${EXECUTABLE_NAME} ${OPENSSL_EXTRA_LIBRARIES})
endif()

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
