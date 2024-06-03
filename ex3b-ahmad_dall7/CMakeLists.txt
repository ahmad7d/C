cmake_minimum_required(VERSION 3.21)
project(ex3b_ahmad_dall7 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(ex3b_ahmad_dall7
        linked_list.c
        linked_list.h
        markov_chain.h
        markov_chain.c
#        snakes_and_ladders.c
        tweets_generator.c
        markov_chain.c
        )
