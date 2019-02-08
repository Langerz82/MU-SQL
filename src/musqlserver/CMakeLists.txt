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
	${CMAKE_SOURCE_DIR}/../muServerDeps/_Include
	${CMAKE_SOURCE_DIR}/dep
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Config
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/Bags
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/CustomEventDrop
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/CustomItemVal
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/CustomMaxStats
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/FloodSystem
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/ItemDropEx
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/ItemOption
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/ItemOptionRate
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/ItemUpgradeJewel
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/Notice
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/PvPBalance
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/PVPZone
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/RandomManager
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/ResetTable
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/SocketItemType
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/UserChatBlockMng
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Customs/UserDieItemDrop
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Attack
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/BattleCore
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Bots
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/BuffSystem
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Calc
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Calc/CharacterSpecialization
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Calc/StatManagement
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/DarkSpirit
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Duels
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/AcheronGuardian
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/ArcaBattle
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/AttackEvent
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/BattleSoccer
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/BloodCastle
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/BonusEvent
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/CastleSiege
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/ChaosCastle
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/ChaosCastleSurvival
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/Crywolf
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/DeepEvent
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/DevilSquare
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/DoppelGangerEvent
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/DragonInvasion
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/Evomon
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/GoldInvasion
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/IllusionTempleLeague
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/IllusionTempleRenewal
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/ImperialGuardianFort
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/Kanturu
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/PrivateEvents
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/PrivateEvents/VictoriaSecret
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/PVPLastManStanding
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/Raklion
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/Rest
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/SantaInvasion
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/TormentedSquare
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Events/WhiteWizard
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ExpManager
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/GameMasters
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Gens
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/GremoryCase
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Guild
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ItemManagement
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ItemManagement/ExcellentAndAccessory
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ItemManagement/HarmonyOptions
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ItemManagement/ItemBags
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ItemManagement/ItemBags/Bags
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ItemManagement/ItemBags/BaseBagClass
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ItemManagement/ItemBags/Lua
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ItemManagement/ItemBags/Manager
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ItemManagement/Lahap
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ItemManagement/PentagramSystem
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ItemManagement/SocketSystem
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/JewelMining
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Maps
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Marry
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Mastering
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/AI
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/AppointDrop
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/AttrAndItemMng
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/gObj
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/Kalima
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/Kalima/Herd
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/KillCount
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/MonsterItemDrop
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/Position
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/Position/Event
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/Position/Regular
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/RegenSystem
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MonsterManagement/Skills
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MuItemShop
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MuRummy
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/MuunSystem
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/NPC
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Party
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/PersonalStore
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Plugins
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Plugins/OfflineLevelling
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/PrivateCustoms
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/ProhibitedSymbols
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Quests
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Quests/QuestExp
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/RuudStore
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/GameSecurity
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Skills
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/User
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/VIP
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Game/Warp
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Lua
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Main
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Main/Lang
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Main/Logs
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Network
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/PrecompiledHeader
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Protection
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Protocol
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/RSA
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Utility
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Utility/CheckSum
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Utility/KeyGenerater
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Utility/MathAuthenticator
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/Utility/ScheduleManager
    ${CMAKE_SOURCE_DIR}/src/musqlserver/old/XML
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

include_directories(${MYSQL_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${MYSQL_LIBRARY})


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
