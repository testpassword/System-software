cmake_minimum_required(VERSION 3.10)
project(mortem VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(SOURCE_FILE
        src/shell.c
        src/client/client.c
        src/models/Book.c
        src/client/ui.c
        src/common/utils.c
        src/server.c
        src/models/frames.c
        src/common/event.c
        src/client/network.c
        )
add_executable(mortem ${SOURCE_FILE})
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
find_package(Threads REQUIRED)
if (CURSES_FOUND AND CURSES_HAVE_NCURSES_H)
    include_directories(${CURSES_INCLUDE_DIR})
    target_link_libraries(mortem PRIVATE Threads::Threads ${CURSES_LIBRARIES} form)
endif ()