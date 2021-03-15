cmake_minimum_required(VERSION 3.17)
project(melon)

set(CMAKE_CXX_STANDARD 17)
SET( CMAKE_CXX_FLAGS  "-DASIO_STANDALONE")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

find_package(spdlog REQUIRED)

file(GLOB_RECURSE PROTOS
        "${CMAKE_SOURCE_DIR}/*.proto"
        )
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOS})

file(GLOB_RECURSE SRC
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/*.h"
        "${CMAKE_SOURCE_DIR}/src/*.pb.*"
        )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(melon ${PROTO_SRCS} ${PROTO_HDRS} ${SRC})

target_link_libraries(melon ${OpenCV_LIBS} ${PROTOBUF_LIBRARIES} spdlog::spdlog)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")
    target_link_libraries(melon ws2_32 wsock32)
endif()