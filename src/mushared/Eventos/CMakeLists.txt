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



file(GLOB SRC_GRP_AcheronGuardianEvent AcheronGuardianEvent/*.cpp AcheronGuardianEvent/*.h)
source_group("AcheronGuardianEvent" FILES ${SRC_GRP_AcheronGuardianEvent})

file(GLOB SRC_GRP_ArcaBattle ArcaBattle/*.cpp ArcaBattle/*.h)
source_group("ArcaBattle" FILES ${SRC_GRP_ArcaBattle})

file(GLOB SRC_GRP_AttackEvent AttackEvent/*.cpp AttackEvent/*.h)
source_group("AttackEvent" FILES ${SRC_GRP_AttackEvent})

file(GLOB SRC_GRP_BloodCastle BloodCastle/*.cpp BloodCastle/*.h)
source_group("BloodCastle" FILES ${SRC_GRP_BloodCastle})

file(GLOB SRC_GRP_BonusEvent BonusEvent/*.cpp BonusEvent/*.h)
source_group("BonusEvent" FILES ${SRC_GRP_BonusEvent})

# OS specific files
if(WIN32)
    file(GLOB SRC_GRP_WIN Win/*.cpp Win/*.h)
    LIST(APPEND LIBRARY_SRCS ${SRC_GRP_WIN})
else()
    file(GLOB SRC_GRP_LINUX Linux/*.cpp Linux/*.h)
    LIST(APPEND LIBRARY_SRCS ${SRC_GRP_LINUX})
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/dep
    ${CMAKE_SOURCE_DIR}/src/framework
    ${CMAKE_BINARY_DIR}
)

