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
# Make sure that SPINNAKER_SDK has been defined
if(NOT DEFINED SPINNAKER_SDK)
    message(FATAL_ERROR "Path to Spinnaker SDK was not set. Set with -DSPINNAKER_SDK=<path>, i.e. -DSPINNAKER_SDK=C:\\Dev\\SpinnakerSdk\\\"")
endif()

# Make sure that an include subdirectory with the header files exists
if(NOT EXISTS ${SPINNAKER_SDK}/include)
    message(FATAL_ERROR "Given Spinnaker SDK path of '${SPINNAKER_SDK}' does not contain an 'include/' subdirectory")
endif()
include_directories(${SPINNAKER_SDK}/include)

###
### NOTE: Checking for the SDK libraries like this is extremely susceptible to breaking, especially if a new version is released
### TODO: More flexible and future-proof way of doing this
###
set(SPINNAKER_LINK_DIR ${SPINNAKER_SDK})
# If lib64 subdirectory exists and system is 64 bit
if((EXISTS ${SPINNAKER_SDK}/lib64) AND (CMAKE_SIZEOF_VOID_P EQUAL 8))
    set(SPINNAKER_LINK_DIR ${SPINNAKER_LINK_DIR}/lib64)
# Otherwise, if lib subdirectory exists
elseif(EXISTS ${SPINNAKER_SDK}/lib)
    set(SPINNAKER_LINK_DIR ${SPINNAKER_LINK_DIR}/lib)
# Otherwise, required libraries to link can't be found
else()
    message(FATAL_ERROR "Given Spinnaker SDK path of '${SPINNAKER_SDK}' does not contain a 'lib/' or 'lib64/' subdirectory")
endif()

# Windows SDK has a vs2015 subdirectory, so check for it
if(EXISTS ${SPINNAKER_LINK_DIR}/vs2015)
    set(SPINNAKER_LINK_DIR ${SPINNAKER_LINK_DIR}/vs2015)
endif()
# Add the link directory
link_directories(${SPINNAKER_LINK_DIR})

# If compiling in debug mode and the debug version of the windows library exists, use it
if(DEBUG AND EXISTS ${SPINNAKER_LINK_DIR}/Spinnakerd_v140.lib)
    set(SPINNAKER_LIBRARIES Spinnakerd_v140)
# Otherwise, if the release version of the windows library exists, use it
elseif(EXISTS ${SPINNAKER_LINK_DIR}/Spinnaker_v140.lib)
    set(SPINNAKER_LIBRARIES Spinnaker_v140)
else()
    # Check if the expected Unix library exists, and emit a warning if not
    if(NOT EXISTS ${SPINNAKER_LINK_DIR}/libSpinnaker.so)
        message(WARNING "Expected library files were not found in ${SPINNAKER_LINK_DIR} -- 'Spinnaker' is being used "
                        "as a default last attempt, but compilation will likely fail")
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
    Configure compilation target
]]
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