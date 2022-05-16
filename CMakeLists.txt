cmake_minimum_required(VERSION 3.15)
project(Smash VERSION 1.0)

set(CMAKE_C_STANDARD 11)
include_directories(includes)

#source files excluding main.c
set(SOURCES parser.c commands.c linenoise.c input.c includes/input.h errorHandler.c includes/errorHandler.h)

#executable name = Smash
add_executable(Smash main.c ${SOURCES})

#GNU readline library used for user input, as line noise is buggy
target_link_libraries(Smash -lreadline)

