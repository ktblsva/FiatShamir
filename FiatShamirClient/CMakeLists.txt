cmake_minimum_required(VERSION 3.23)
project(FiatShamirClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(FiatShamirClient main.cpp)
target_link_libraries(FiatShamirClient pq)
