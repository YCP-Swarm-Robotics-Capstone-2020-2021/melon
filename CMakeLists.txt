cmake_minimum_required(VERSION 3.17)
project(melon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DASIO_STANDALONE")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

find_package(spdlog REQUIRED)

if(NOT DEFINED SPINNAKER_SDK)
    message(FATAL_ERROR "Path to Spinnaker SDK was not set. Set with -DSPINNAKER_SDK=<path>, i.e. -DSPINNAKER_SDK=C:\\Dev\\SpinnakerSdk\\\"")
endif()

include_directories(${SPINNAKER_SDK}/include)
# Use "lib64/" directory if 64 bit target, otherwise use "lib/"
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    link_directories(${SPINNAKER_SDK}/lib64/vs2015)
else()
    link_directories(${SPINNAKER_SDK}/lib/vs2015)
endif()
if(DEBUG)
    set(SPINNAKER_LIBRARIES Spinnakerd_v140)
else()
    set(SPINNAKER_LIBRARIES Spinnaker_v140)
endif()

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
target_link_libraries(melon ${OpenCV_LIBS} ${PROTOBUF_LIBRARIES} spdlog::spdlog ${SPINNAKER_LIBRARIES})
if(NOT MSVC)
    target_link_libraries(melon stdc++fs)
endif()

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")
    target_link_libraries(melon ws2_32 wsock32)
endif()