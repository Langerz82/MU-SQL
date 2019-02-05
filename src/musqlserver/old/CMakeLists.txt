#MuMySQL Server Files
file(GLOB_RECURSE SRC_GRP_MUSQLSERVER_OLD *.cpp *.h *.hpp)
source_group("old" FILES ${SRC_GRP_MUSQLSERVER_OLD})

file(GLOB_RECURSE SRC_GRP_MUSQLSERVER_OLD_ITEM Item/*.cpp Item/*.h Item/*.hpp)
source_group("item" FILES ${SRC_GRP_MUSQLSERVER_OLD_ITEM})
