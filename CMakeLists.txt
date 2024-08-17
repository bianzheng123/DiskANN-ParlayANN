cmake_minimum_required(VERSION 3.27)
project(DiskANN_ParlayANN)

set(CMAKE_CXX_STANDARD 14)

find_package(Parlay REQUIRED)

#add_subdirectory(parlaylib)
include_directories(parlaylib/include)

add_executable(main_build_index src/build_index.cpp)
target_link_libraries(main_build_index PRIVATE Parlay::parlay)

add_executable(main_search src/search.cpp)
target_link_libraries(main_search PRIVATE Parlay::parlay)

add_executable(main_cpy src_cpy/neighborsTime.cpp)
target_link_libraries(main_cpy PRIVATE Parlay::parlay)

add_executable(main_euc_build_index src_euclidean/build_index.cpp)
target_link_libraries(main_euc_build_index PRIVATE Parlay::parlay)

add_executable(main_euc_search src_euclidean/search.cpp)
target_link_libraries(main_euc_search PRIVATE Parlay::parlay)
