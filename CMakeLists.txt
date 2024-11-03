# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(ChessEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(include)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Create the executable
add_executable(ChessEngine ${SOURCES})

# Link any third-party libraries if needed
# Example: target_link_libraries(ChessEngine some_library)