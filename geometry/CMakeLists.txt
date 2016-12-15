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

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

add_library(geometryGenerator SHARED ${SOURCES})

