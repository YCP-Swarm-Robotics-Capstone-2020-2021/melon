cmake_minimum_required(VERSION 3.17)
project(melon)

set(CMAKE_CXX_STANDARD 17)
#SET( CMAKE_CXX_FLAGS  "-pthread -lprotobuf -lpthread")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

file(GLOB_RECURSE PROTOS
        "${CMAKE_SOURCE_DIR}/*.proto"
        )

file(GLOB_RECURSE SRC
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/*.h"
        "${CMAKE_SOURCE_DIR}/src/*.pb.*"
        )
add_executable(melon ${SRC})

target_link_libraries(melon ${OpenCV_LIBS} ${PROTOBUF_LIBRARIES})

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")
    target_link_libraries(melon ws2_32 wsock32)
endif()