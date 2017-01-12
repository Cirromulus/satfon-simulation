MESSAGE(STATUS "simu")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
set (SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/flashCell.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/debugInterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/debugInterfaceClient.cpp
)

COMPILE_AS_CXX11()

find_package (Threads)


set(CMAKE_CXX_FLAGS_DEBUG 
        "${CMAKE_CXX_FLAGS_DEBUG} -Wall"
)

add_library(simu ${SOURCES})
target_link_libraries (simu ${CMAKE_THREAD_LIBS_INIT})

add_executable(simu_example tests/example.cpp)
target_link_libraries(simu_example simu)