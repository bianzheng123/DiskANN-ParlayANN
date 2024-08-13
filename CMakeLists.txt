cmake_minimum_required(VERSION 3.27)
project(DiskANN_ParlayANN)

set(CMAKE_CXX_STANDARD 14)

find_package(Parlay REQUIRED)

#add_subdirectory(parlaylib)
include_directories(parlaylib/include)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE Parlay::parlay)

add_executable(main_cpy src_cpy/neighborsTime.cpp)
target_link_libraries(main_cpy PRIVATE Parlay::parlay)

