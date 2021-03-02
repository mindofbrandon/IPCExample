cmake_minimum_required(VERSION 3.17)
project(IPCexample C)

set(CMAKE_C_STANDARD 99)

add_executable(producer producer.c)
target_link_libraries(producer -lrt) # not linking -lrt will cause program to fail in editor

add_executable(consumer consumer.c)
target_link_libraries(consumer -lrt)