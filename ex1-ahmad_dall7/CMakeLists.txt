cmake_minimum_required(VERSION 3.21)
project(ex1_ahmad_dall7 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(ex1_ahmad_dall7
        cipher.c
        cipher.h
        main.c
        tests.c
        tests.h)
