cmake_minimum_required(VERSION 3.17)
project(melon)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#add_executable(melon src/main.cpp)
add_executable(melon src/main_calibration.cpp)

target_link_libraries(melon ${OpenCV_LIBS})