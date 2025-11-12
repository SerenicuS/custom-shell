cmake_minimum_required(VERSION 4.0)
project(MyShell C)

set(CMAKE_C_STANDARD 11)

add_executable(MyShell main.c)

set_target_properties(MyShell PROPERTIES OUTPUT_NAME "dMommyShell")