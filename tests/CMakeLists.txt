add_definitions(-std=c++11 -Wall -Wunused-parameter -Wextra)

include_directories(${INCLUDE_PARENT})
add_definitions(${DEFINITIONS_PARENT})
link_directories(${LIBRARY_PARENT})

add_executable(GeometryVisualisatorTest.exe GeometryVisualisatorTest.cpp)
target_link_libraries(GeometryVisualisatorTest.exe  ${Boost_LIBRARIES} ${ROOT_LIBRARIES}  )
