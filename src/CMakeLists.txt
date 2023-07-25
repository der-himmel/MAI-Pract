cmake_minimum_required(VERSION 3.13)
project(pract-final)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)

# Find required packages
find_package(Boost REQUIRED COMPONENTS asio)
find_package(OpenCV REQUIRED)

set(SOURCES pract-final/main.cpp Cam.h Cam.cpp RangeF.cpp RangeF.h Contr. Contr.cpp)
add_executable(main ${SOURCES})

# Add include directories
target_include_directories(pract-final PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(pract-final
    PRIVATE
    ${Boost_LIBRARIES}
    ${OpenCV_LIBS}
)
