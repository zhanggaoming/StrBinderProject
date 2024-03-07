cmake_minimum_required(VERSION 3.10)
project(StrBinderProject C)

set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(StrService binder.c StrService.c)

add_executable(StrClient binder.c StrClient.c)