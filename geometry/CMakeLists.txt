add_definitions(-std=c++11 -Wall -Wunused-parameter -Wextra)

include_directories(${INCLUDE_PARENT})
add_definitions(${DEFINITIONS_PARENT})
link_directories(${LIBRARY_PARENT})

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

add_library(geometryGenerator SHARED ${SOURCES})

