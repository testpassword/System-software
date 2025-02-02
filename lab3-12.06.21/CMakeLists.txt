cmake_minimum_required(VERSION 3.10)
project(mortem VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(SOURCE_FILE
        src/shell.c
        src/controllers/client.c
        src/models/Book.c
        src/views/ui_components.c
        src/str_extensions.c
        src/controllers/server.c
        src/dtos/BookFrame.c
        src/dtos/Frame.c
        src/models/Command.c
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