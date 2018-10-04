cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(parking_lot)

file(GLOB_RECURSE LIB_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${LIB_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/)




