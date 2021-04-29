cmake_minimum_required(VERSION 3.19)
project(Spo_1 C)

set(CMAKE_C_STANDARD 99)

add_executable(Spo_1 sources/main.c headers/main.h sources/hfsplus_utils.c headers/hfsplus_utils.h headers/hfs_plus_data_structures.h sources/hfs_plus_data_structures.c sources/partitions.c)