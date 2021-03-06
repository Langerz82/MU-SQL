set(EXECUTABLE_NAME connectserver)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

file(GLOB_RECURSE _source_list *.c* *.h*)
foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/src/connectserver" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
	set(MU_INCLUDE_DIRS "${MU_INCLUDE_DIRS};${CMAKE_SOURCE_DIR}\\src\\connectserver${_group_path}")
endforeach()

set(EXECUTABLE_SRCS
    ${_source_list}
    Main.cpp
)

if(WIN32)
    include(generate_product_version)
    generate_product_version(
        VersionFilesOutputVariable
        NAME "connectserver.exe"
        ICON ${CMAKE_SOURCE_DIR}/src/musqlserver/realmd.ico
        VERSION_MAJOR 0
        VERSION_MINOR 21
        VERSION_PATH ${BUILD_COUNTER}
        VERSION_REVISION ${BUILD_REVISION}
        COMPANY_NAME ""
        # Do not alter, remove or prevent COMPANY_COPYRIGHT from displaying
        COMPANY_COPYRIGHT
        FILE_DESCRIPTION "Mu MySQL Connect Server Daemon"
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
	${CMAKE_SOURCE_DIR}/../muServerDeps/_Include
	${CMAKE_SOURCE_DIR}/dep
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/connectserver
	${CMAKE_SOURCE_DIR}/src/connectserver/old
	${MU_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${MYSQL_INCLUDE_DIR}
)

link_directories(
	${Boost_LIBRARY_DIRS}
	${CMAKE_SOURCE_DIR}/../muServerDeps
	${CMAKE_SOURCE_DIR}/../muServerDeps/v140_x64_MT
	${Boost_INCLUDE_DIRS}/lib64-msvc-14.0
	${CMAKE_SOURCE_DIR}/build/src/shared
)

add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SRCS})

target_link_libraries(${EXECUTABLE_NAME}
	shared
    framework
	fmt
	ACE
	${OPENSSL_LIBRARIES}
	${Boost_LIBRARIES}
	${MYSQL_LIBRARY}
)

if (WIN32)
	include_directories(${Boost_INCLUDE_DIRS})
	target_link_libraries(${EXECUTABLE_NAME} ${BOOST_LIBRARY})
endif()

if (UNIX)
target_link_libraries(${EXECUTABLE_NAME}
	boost_system
	boost_filesystem
	pthread
)
endif()

#include_directories(${MYSQL_INCLUDE_DIR})
#target_link_libraries(${EXECUTABLE_NAME} ${MYSQL_LIBRARY})


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


if (WIN32)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${BIN_DIR}")
endif()

install(TARGETS ${EXECUTABLE_NAME} DESTINATION "${BIN_DIR}")

set_target_properties(${EXECUTABLE_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${BIN_DIR}/connectserver"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_DIR}/connectserver"
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}/connectserver"
)

if(WIN32 AND MSVC)
    install(FILES $<TARGET_PDB_FILE:${EXECUTABLE_NAME}> DESTINATION ${BIN_DIR} OPTIONAL)
endif()



