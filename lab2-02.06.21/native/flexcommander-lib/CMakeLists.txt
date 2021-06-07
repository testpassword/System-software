cmake_minimum_required(VERSION 3.16)
project(flexcommander-lib C)
add_compile_definitions(HFSPlus)
include_directories("include")
include_directories("src/include")
set(CMAKE_C_STANDARD 11)
add_library(flexcommander-lib SHARED
        include/FlexIO.h
        src/FlexIO.c
        include/Flexcommander.h src/Init.c src/Devices.c include/HFSPlus.h src/FSHFSPlus.c
        include/HFSPlusBTree.h src/BTreePrint.c include/HFSCatalog.h src/BTree.c src/utils/Endians.h src/utils/Endians.c src/HFSCatalog.c src/include/List.h src/include/List.c src/copy/Copy.c src/copy/Copy.h src/utils/Utils.h src/utils/Utils.c src/utils/BTreeUtils.h src/utils/BTreeUtils.c include/FlexCommanderFS.h)
target_link_libraries(flexcommander-lib blkid)
