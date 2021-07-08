cmake_minimum_required(VERSION 3.15)
project(Smash VERSION 1.0)

set(CMAKE_C_STANDARD 11)
include_directories(includes)

#source files excluding main.c
set(SOURCES parser.c commands.c linenoise.c)

#executable name = Smash
add_executable(Smash main.c ${SOURCES})

