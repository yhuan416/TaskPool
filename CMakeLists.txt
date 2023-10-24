cmake_minimum_required(VERSION 3.5)

project(TaskPool)

add_library(TaskPool STATIC src/TaskPool.c)
target_include_directories(TaskPool PUBLIC include)

add_subdirectory(ual)
target_link_libraries(TaskPool ual)

add_executable(TaskPoolTest test/main.c)
target_link_libraries(TaskPoolTest TaskPool)
target_link_libraries(TaskPoolTest pthread)
