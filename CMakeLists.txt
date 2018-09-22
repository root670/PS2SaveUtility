cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(PS2SaveUtility)

#
# Source files
#
file(GLOB sources
    "src/*.cpp"
    "src/*.h"
)
add_executable(PS2SaveUtility ${sources})

#
# Dependencies
#
find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries(PS2SaveUtility ${ZLIB_LIBRARIES})
endif()
