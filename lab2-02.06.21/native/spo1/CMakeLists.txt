cmake_minimum_required(VERSION 3.16)
project(spo1 C)
add_compile_definitions(HFSPlus)
include_directories("include")
include_directories("src/include")
set(CMAKE_C_STANDARD 11)
add_library(spo1 SHARED
        include/IOFunc.h
        src/IOFunc.c
        include/Commander.h include/HFSPlus.h src/FSHFSPlus.c
        include/HFSPlusBTree.h include/HFSCatalog.h src/BTree.c src/utils/Endians.h src/utils/Endians.c src/HFSCatalog.c src/include/List.h src/include/List.c src/copy/Copy.c src/copy/Copy.h src/utils/BTreeUtils.h src/utils/BTreeUtils.c)
target_link_libraries(spo1 blkid)