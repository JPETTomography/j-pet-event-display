cmake_minimum_required(VERSION 2.6)
project(JPET_Event_Display CXX) # using only C++

add_subdirectory(j-pet-framework)
include_directories(j-pet-framework ${Framework_INCLUDE_DIRS})
add_definitions(${Framework_DEFINITIONS})

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

find_package(Boost 1.41.0 REQUIRED COMPONENTS
  filesystem
  program_options
  regex
  system
  unit_test_framework
  )

set(INCLUDE_PARENT ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} )
set(DEFINITIONS_PARENT ${ROOT_DEFINITIONS} ${Boost_DEFINITIONS} )
set(LIBRARY_PARENT ${ROOT_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS} )

add_subdirectory(src)
add_subdirectory(tests)
#add_subdirectory(geometry)

add_definitions(-std=c++11 -Wall -Wunused-parameter -Wextra -pthread)


include_directories(${INCLUDE_PARENT})
add_definitions(${DEFINITIONS_PARENT})
link_directories(${LIBRARY_PARENT})

add_executable(EventDisplay.exe ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(EventDisplay.exe eventDisplay JPetFramework ${Boost_LIBRARIES} ${ROOT_LIBRARIES})

set(DOWNLOAD_DATA ${CMAKE_CURRENT_SOURCE_DIR}/download_data.sh ${CMAKE_CURRENT_BINARY_DIR})
if(NOT DOWNLOAD_DATA_HAPPENED_EVENT_DISPLAY)
  execute_process(COMMAND ${DOWNLOAD_DATA})
  set(DOWNLOAD_DATA_HAPPENED_EVENT_DISPLAY TRUE CACHE BOOL "Has the download happened?" FORCE)
endif()