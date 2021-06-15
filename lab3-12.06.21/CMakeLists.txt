cmake_minimum_required(VERSION 3.10)
project(spo_lab3 VERSION 1.0 DESCRIPTION "Lab3 for SPO" LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(SOURCE_FILE
        src/main.c
        src/client/client.c
        src/common/book.c
        src/client/ui.c
        src/common/utils.c
        src/client/clientEvent.c
        src/server/server.c
        src/common/protocol.c
        src/server/serverEvent.c
        src/common/event.c
        src/server/network.c
        src/client/network.c
        )
add_executable(spo_lab3 ${SOURCE_FILE})
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
find_package(Threads REQUIRED)
if (CURSES_FOUND AND CURSES_HAVE_NCURSES_H)
    include_directories(${CURSES_INCLUDE_DIR})
    target_link_libraries(spo_lab3 PRIVATE Threads::Threads ${CURSES_LIBRARIES} form)
endif ()