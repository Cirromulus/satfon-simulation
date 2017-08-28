PROJECT(FlashView)
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)

INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/GeneratedFiles
)

SET(SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/flashviewer.cpp
)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(flashView main.cpp ${SOURCES})
target_link_libraries(flashView simu Qt5::Widgets)
