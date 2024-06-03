cmake_minimum_required(VERSION 3.21)
project(ex3a_ahmad_dall7)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(ex3a_ahmad_dall7
        linked_list.c
        tweets_generator.c
        linked_list.h
        markov_chain.h
        markov_chain.c
        README.md
        tweets_generator.c)
