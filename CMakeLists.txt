cmake_minimum_required(VERSION 3.17)
project(melon)

set(CMAKE_CXX_STANDARD 17)
SET( CMAKE_CXX_FLAGS  "-pthread")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#add_executable(melon src/main.cpp)
#add_executable(melon src/main_calibration.cpp)
add_executable(melon src/server.cpp)

target_link_libraries(melon ${OpenCV_LIBS})