project(FINS)
cmake_minimum_required(VERSION 2.6)

set(FINS_SRCS FINS.c main.cpp)

add_library(FINS SHARED ${FINS_SRCS})