# MuMySQLServer is a full featured server for World of Warcraft, supporting
# the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
#
# Copyright (C) 2005-2019  MuMySQLServer project <https://getmangos.eu>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

set (CMAKE_SYSTEM_VERSION 8.1 CACHE TYPE INTERNAL FORCE)
project(MuMySQLServer)
cmake_minimum_required(VERSION 3.0)

# Set the correct macro directory path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# INCLUDE(${CMAKE_MODULE_PATH}/FindMySQL.cmake)
# INCLUDE(${CMAKE_MODULE_PATH}/FindPostgreSQL.cmake)
INCLUDE(${CMAKE_MODULE_PATH}/FindBoost.cmake)

# MESSAGE("-- Check PostgreSQL")
# FIND_PGSQL()
# ADD_DEFINITIONS(-DDO_POSTGRESQL)

# MESSAGE("-- Check MySQL")
# FIND_MYSQL()
# ADD_DEFINITIONS(-DDO_MYSQL)

# Define available cmake options below
if(WIN32)	
    set(CONF_DIR "" CACHE STRING "Path to the configs, can be absolute or relative.")
else()
    set(CONF_DIR "etc/" CACHE STRING "Path to the configs, can be absolute or relative.")
endif()
option(DEBUG                "Debug mode (strict compile, all warnings)"     OFF)
option(ACE_USE_EXTERNAL     "Use external ACE"                              OFF)
option(BUILD_CONNECTSERVER        "Build the Connect server"                     ON)
option(BUILD_GAMESERVER        "Build the Game server"                     ON)
option(SOAP                 "Enable remote access via SOAP"                 OFF)
# Hidden option to enable/disable PCH. DEV ONLY!
set(PCH ON)

# Print CLI helper message
message("")
message(
  "This script builds the MuMySQLServer server.
  Options that can be used in order to configure the process:
   General:
    CMAKE_INSTALL_PREFIX    Path where the server should be installed to
    CONF_DIR                Path to the configs, can be absolute or relative.
    DEBUG                   Debug mode (strict compile, all warnings)
    ACE_USE_EXTERNAL        Use external ACE
    BUILD_MUSQLSERVER       Build the data server
    SOAP                    Enable remote access via SOAP
  To set an option simply type -D<OPTION>=<VALUE> after 'cmake <srcs>'.
  Also, you can specify the generator with -G. see 'cmake --help' for more details
  For example: cmake .. -DDEBUG=1 -DCMAKE_INSTALL_PREFIX=/opt/mangos

  Note: On UNIX systems, CONF_DIR is relative to the bin folder."
)
message("")

# Search for and set up the needed packages
set(OPENSSL_EXPECTED_VERSION 1.1.0)

find_package(Platform REQUIRED)
find_package(Git)
find_package(PCHSupport)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED)

if(ACE_USE_EXTERNAL)
    find_package(ACE)
    if(NOT ACE_FOUND)
        message(FATAL_ERROR
            "This project requires ACE installed when ACE_USE_EXTERNAL is set. Please download the ACE Micro Release Kit from http://download.dre.vanderbilt.edu/ and install it. If this script didn't find ACE and it was correctly installed please set ACE_ROOT to the correct path."
        )
    endif()
else()
    include(cmake/ImportACE.cmake)
endif()

if(POSTGRESQL)
    find_package(PostgreSQL REQUIRED)
else()
    find_package(MySQL REQUIRED)
endif()

if(UNIX)
    find_package(ZLIB)
    find_package(BZip2)
endif()

#Include platform/compiler-specific definitions
include(${CMAKE_SOURCE_DIR}/cmake/SetDefinitions.cmake)

add_definitions(-DCLASSIC)

message(STATUS "Install server to     : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Install configs to    : ${CONF_INSTALL_DIR}")
if("${CONF_DIR}" STREQUAL "")
    message(STATUS "Search configs from   : binary directory (default)")
else()
    message(STATUS "Search configs from   : ${CONF_DIR}")
endif()

if(SOAP)
    message(STATUS "Support for SOAP      : Yes")
    add_definitions(-DENABLE_SOAP)
else()
    message(STATUS "Support for SOAP      : No (default)")
endif()

if(DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Build in debug-mode   : Yes")
else()
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Build in debug-mode   : No  (default)")
endif()

# Add dependency path
# if(BUILD_MUSQLSERVER)
    add_subdirectory(dep)
	#a dd_subdirectory(dep/acelite)
# else()
    # add_subdirectory(dep/acelite)
# endif()

# Add source path
add_subdirectory(src)
