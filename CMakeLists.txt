cmake_minimum_required(VERSION 3.10)
project(BinderC C)

set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR})

#add_executable(BinderC binder.c main.c HelloServer.c HelloClient.c)

add_executable(HelloServer binder.c  HelloServer.c)

add_executable(HelloClient binder.c  HelloClient.c)
