add_definitions(-std=c++11 -Wall -Wunused-parameter -Wextra)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(${INCLUDE_PARENT})
add_definitions(${DEFINITIONS_PARENT})
link_directories(${LIBRARY_PARENT})

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

set(DICTIONARY_REQUIRED
  EventDisplay.h
  )

set(LINKDEF_NAME GuiLinkDef.h)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Dictionaries)
set(DICTIONARY_NAME
        ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Dictionaries/JPetEventDisplay_Dictionary)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
ROOT_GENERATE_DICTIONARY(${DICTIONARY_NAME} ${DICTIONARY_REQUIRED}
  LINKDEF ${LINKDEF_NAME}
  OPTIONS -p)
list(APPEND SOURCES ${DICTIONARY_NAME}.cxx)

add_library(eventDisplay SHARED ${SOURCES})
