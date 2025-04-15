cmake_minimum_required(VERSION 3.10)

# Set the C and C++ compiler paths explicitly
set(CMAKE_C_COMPILER "C:/gcc-14.2.0/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/gcc-14.2.0/bin/g++.exe")

project(LockFreeDataStructures)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure we're building for the correct architecture
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/src/include)

# Add source files
set(SOURCES
    main.cpp
    src/skip_list.cpp
)

# Create executable
add_executable("out" ${SOURCES})

# Set build types
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(out PRIVATE -g)
else()
    target_compile_options(out PRIVATE -O2)
endif()

# Add compiler flags
target_compile_options(out PRIVATE
    -Wall
    -Wextra
    -std=c++20
    -pedantic
    -Wold-style-cast
    -Woverloaded-virtual
    -Wsign-promo 
    -Wctor-dtor-privacy
    -Wnon-virtual-dtor
    -Wreorder 
)

# Add flag to statically link the standard library
target_link_libraries(out PRIVATE -static-libgcc -static-libstdc++)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})