add_definitions(-std=c++11 -Wall -Wunused-parameter -Wextra)

find_package(ROOT REQUIRED COMPONENTS 
  Gui
  Rint
  Geom 
  GeomPainter 
  TreePlayer  
  Hist
  HistPainter
  Physics
  RIO
  Thread
  Gpad
  Tree
  Html
  Core
  )
include_directories(${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_DEFINITIONS})
link_directories(${ROOT_LIBRARY_DIRS})

find_package(Boost 1.41.0 REQUIRED COMPONENTS
  filesystem
  program_options
  regex
  system
  unit_test_framework
  )
include_directories(${Boost_INCLUDE_DIRS})
add_definitions(${Boost_DEFINITIONS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(GeometryVisualisatorTest.exe GeometryVisualisatorTest.cpp)
target_link_libraries(GeometryVisualisatorTest.exe  ${Boost_LIBRARIES} ${ROOT_LIBRARIES}  )

add_executable(TreeReaderTest.exe TreeReaderTest.cpp)
target_link_libraries(TreeReaderTest.exe eventDisplay  ${Boost_LIBRARIES} ${ROOT_LIBRARIES}  )

add_custom_command(TARGET TreeReaderTest.exe POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/tests/testData ${CMAKE_CURRENT_BINARY_DIR}/testData)
