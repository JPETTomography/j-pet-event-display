add_definitions(-std=c++11 -Wall -Wunused-parameter -Wextra)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(JPetFramework-Utilities) # for generate_root_dictionaries
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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Dictionaries/)
set(DICT ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Dictionaries/GuiDictionary.cxx) 

add_custom_command(OUTPUT ${DICT}
  COMMAND rootcint 
  -f ${DICT} 
  -c 
  ${CMAKE_CURRENT_SOURCE_DIR}/EventDisplay.h
  ${CMAKE_CURRENT_SOURCE_DIR}/GuiLinkDef.h )
add_custom_target(disttarget DEPENDS ${DICT})

add_library(eventDisplay SHARED ${SOURCES} ${DICT})
add_dependencies(eventDisplay disttarget)
set_source_files_properties(
    ${DICT}
    PROPERTIES GENERATED TRUE
    )
