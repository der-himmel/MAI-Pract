cmake_minimum_required(VERSION 3.0)
project(streaming)

# Find required packages (Boost and OpenCV)
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenCV REQUIRED)

# Server executable
add_executable(server server.cpp)
target_link_libraries(server ${Boost_LIBRARIES} ${OpenCV_LIBS})
