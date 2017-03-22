add_definitions(-std=c++11 -Wall -Wunused-parameter -Wextra)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(JPetFramework-Utilities) # for generate_root_dictionaries

include_directories(${INCLUDE_PARENT})
add_definitions(${DEFINITIONS_PARENT})
link_directories(${LIBRARY_PARENT})

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Dictionaries/)
set(DICT ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Dictionaries/GuiDictionary.cxx) 

add_custom_command(OUTPUT ${DICT}
  COMMAND rootcint 
  -f ${DICT} 
  -c 
  ${CMAKE_CURRENT_SOURCE_DIR}/EventDisplay.h
  ${CMAKE_CURRENT_SOURCE_DIR}/GuiLinkDef.h
  DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/EventDisplay.h
  ${CMAKE_CURRENT_SOURCE_DIR}/GuiLinkDef.h )
add_custom_target(disttarget DEPENDS ${DICT})

add_library(eventDisplay SHARED ${SOURCES} ${DICT})
add_dependencies(eventDisplay disttarget)
set_source_files_properties(
    ${DICT}
    PROPERTIES GENERATED TRUE
    )
