cmake_minimum_required(VERSION 3.17)
project(melon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DASIO_STANDALONE")

#[[
    Find OpenCV
]]
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#[[
    Find Google Protobuf
]]
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

#[[
    Find spdlog
]]
find_package(spdlog REQUIRED)

#[[
    Find and configure Spinnaker
]]
if(NOT DEFINED SPINNAKER_INCLUDE_DIR)
    message(FATAL_ERROR "Path to Spinnaker include directory was not set. This directory should contain the Spinnaker SDK Header files. "
                        "Set with -DSPINNAKER_INCLUDE_DIR=<path>, i.e. -DSPINNAKER_INCLUDE_DIR=C:\\Dev\\SpinnakerSdk\\include\\\"")
endif()
include_directories(${SPINNAKER_INCLUDE_DIR})

if(NOT DEFINED SPINNAKER_LIB_DIR)
    message(FATAL_ERROR "Path to Spinnaker lib directory was not set. This directory should contain the Spinnaker SDK lib files. "
            "Set with -DSPINNAKER_LIB=<path>, i.e. -DSPINNAKER_LIB=C:\\Dev\\SpinnakerSdk\\lib\\\"")
endif()
link_directories(${SPINNAKER_LIB_DIR})

# If compiling in debug mode and the debug version of the windows library exists, use it
if(DEBUG AND EXISTS ${SPINNAKER_LIB_DIR}/Spinnakerd_v140.lib)
    set(SPINNAKER_LIBRARIES Spinnakerd_v140)
# Otherwise, if the release version of the windows library exists, use it
elseif(EXISTS ${SPINNAKER_LIB_DIR}/Spinnaker_v140.lib)
    set(SPINNAKER_LIBRARIES Spinnaker_v140)
else()
    # Check if the expected Unix library exists, and emit a warning if not
    if(NOT EXISTS ${SPINNAKER_LIB_DIR}/libSpinnaker.so)
        message(WARNING "Expected library files were not found in ${SPINNAKER_LIB_DIR} -- 'Spinnaker' is being used "
                        "as a default last attempt, but compilation will likely fail. If compiling for MSVC, make sure "
                        "that your SPINNAKER_LIB_DIR doesn't have additional sub-directories between your given directory "
                        "and any *.lib files")
    endif()

    set(SPINNAKER_LIBRARIES Spinnaker)
endif()

#[[
    Compile protobuf messages
]]
file(GLOB_RECURSE PROTOS
        "${CMAKE_SOURCE_DIR}/*.proto"
        )
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOS})

#[[
    Gather all source files
]]
file(GLOB_RECURSE SRC
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/*.h"
        "${CMAKE_SOURCE_DIR}/src/*.pb.*"
        )

#[[
    Add googletest dependency
]]
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Gather all test files
file(GLOB_RECURSE TESTS
        "${CMAKE_SOURCE_DIR}/tests/*.cc"
        "${CMAKE_SOURCE_DIR}/src/cmdhandler/command_handler.*"
        )

add_executable(AllTests ${PROTO_SRCS} ${PROTO_HDRS} ${TESTS})
target_link_libraries(AllTests gtest gtest_main gmock ${OpenCV_LIBS} ${PROTOBUF_LIBRARIES} spdlog::spdlog stdc++fs)
add_test(NAME AllTests COMMAND AllTests)

# Add the compile directory to the include directories so that the generated protobuf classes can be found
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# Add compilation target
add_executable(melon ${PROTO_SRCS} ${PROTO_HDRS} ${SRC})
# Link libraries to compilation target
target_link_libraries(melon ${OpenCV_LIBS} ${PROTOBUF_LIBRARIES} spdlog::spdlog ${SPINNAKER_LIBRARIES})
# If MSVC compiler is not being used, add stdc++fs library
if(NOT MSVC)
    target_link_libraries(melon stdc++fs)
endif()

# If on windows, add a special compiler definition for Asio and include Windows socket libraries for Asio
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")
    target_link_libraries(melon ws2_32 wsock32)
endif()