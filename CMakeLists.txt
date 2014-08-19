cmake_minimum_required(VERSION 2.8.12)

project(BTSyncQt)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(CXX11 REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")

add_subdirectory(libbtsync-qt)
add_subdirectory(btsync-qt)
