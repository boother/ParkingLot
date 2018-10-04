cmake_minimum_required(VERSION 3.1)

project(parking_lot_app)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(lib)

set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/app/app.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} parking_lot) #${GRAPH_LIB}


