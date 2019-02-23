cmake_minimum_required(VERSION 2.6)
project(JPET_Event_Display CXX) # using only C++

add_subdirectory(j-pet-framework)
include_directories(j-pet-framework ${Framework_INCLUDE_DIRS})
add_definitions(${Framework_DEFINITIONS})

################################################################################
## Find CERN ROOT
# first try to find ROOT 6 or ROOT 5 compiled with CMake
# if present, such ROOT should be possible to find without explicit setting of any paths
find_package(ROOT 5 QUIET COMPONENTS 
  Gui
  Geom )

if(ROOT_FOUND)
  message(STATUS "ROOT (version ${ROOT_VERSION}) was found using ROOTConfig")
else()
  # try to locate ROOT using ROOTSYS and root-config in case ROOTSYS is not set
  # this might be necessary for ROOT 5 built with autotools rather than CMake
  set(root_prefix $ENV{ROOTSYS})
  if(root_prefix)
    list(APPEND CMAKE_MODULE_PATH ${root_prefix}/etc/cmake/)
  else()
    execute_process(COMMAND root-config --etcdir OUTPUT_VARIABLE ROOT_ETCDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    list(APPEND CMAKE_MODULE_PATH ${ROOT_ETCDIR}/cmake)
  endif()
  find_package(ROOT 5 QUIET)
  if(ROOT_FOUND)
    message(STATUS "ROOT (version ${ROOT_VERSION}) was found using FindROOT (legacy mode)")
  else()
    # as last resort, use the bundled FindROOT module
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/j-pet-framework/cmake/fallback)
    find_package(ROOT 5 REQUIRED)
    if(ROOT_FOUND)
      message(WARNING "No CMake modules from a ROOT installation were found. ROOT (version ${ROOT_VERSION}) was only found using fallback mode)")
    endif()
  endif()
endif()

if(ROOT_USE_FILE)
  include(${ROOT_USE_FILE})
endif()

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