cmake_minimum_required(VERSION 2.6)
project(JPET_Event_Display CXX) # using only C++
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(geometry)

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

add_executable(EventDisplay.exe ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_executable(createGeometryPET.exe ${CMAKE_CURRENT_SOURCE_DIR}/geometry/createGeometryPET.cpp)
target_link_libraries(EventDisplay.exe  eventDisplay ${Boost_LIBRARIES} ${ROOT_LIBRARIES})
target_link_libraries(createGeometryPET.exe  geometryGenerator ${Boost_LIBRARIES} ${ROOT_LIBRARIES})

add_custom_command(
  OUTPUT JPET_geom.root
  COMMAND createGeometryPET.exe
  DEPENDS 
  COMMENT "Create geometry for visualizator"
  VERBATIM
)

add_custom_target(
  CreateGeometry ALL
  DEPENDS JPET_geom.root
)
