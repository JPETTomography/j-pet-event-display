cmake_minimum_required(VERSION 3.1...3.14)

if(${CMAKE_VERSION} VERSION_LESS 3.14)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.14)
endif()

project(JPetEventDispay VERSION 1.0.0
                  LANGUAGES CXX)
set(PROJECT_DESCRIPTION "JPetEventDispay module")

message(STATUS "")
message(STATUS "    == ${PROJECT_NAME} Project configuration ==")
message(STATUS "")

set(CMAKE_CXX_STANDARD 11) # maybe not best solution to set it globally? upgrate minimal cmake version to 3.8 or find another way to set it only for target with interface
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PLATFORM_INDEPENDENT_CODE ON)

# Force out-of-source build
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Be nice and export compile commands by default, this is handy for clang-tidy
# and for other tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Helpful option enable build profiling to identify slowly compiling files
option(MEASURE_ALL "When enabled all commands will be passed through time command" OFF)
if(MEASURE_ALL)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "time")
endif()

################################################################################
## Find CERN ROOT 6.10+
find_package(ROOT CONFIG REQUIRED)
if(ROOT_FOUND)
  message(STATUS "ROOT (version ${ROOT_VERSION}) was found using ROOTConfig")
endif()

if(EXISTS "${ROOT_DIR}/modules/RootNewMacros.cmake")
  include("${ROOT_DIR}/modules/RootNewMacros.cmake")
  message(STATUS "Found RootNewMacros.cmake in: ${ROOT_DIR}/modules!")
elseif(EXISTS "${ROOT_DIR}/cmake/modules/RootNewMacros.cmake")
  include("${ROOT_DIR}/cmake/modules/RootNewMacros.cmake")
  message(STATUS "Found RootNewMacros.cmake in: ${ROOT_DIR}/cmake/modules!")
elseif(EXISTS "${ROOT_DIR}/../cmake/modules/RootNewMacros.cmake")
  include("${ROOT_DIR}/../cmake/modules/RootNewMacros.cmake")
  message(STATUS "Found RootNewMacros.cmake in: ${ROOT_DIR}/../cmake/modules!")
else()
  message("ERROR! Could not find RootNewMacros.cmake file! Aborting..")
  return(1)
endif()
# fix missing include directories property in versions < ROOT 6.12
set_target_properties(ROOT::Core PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ROOT_INCLUDE_DIRS}")


add_library(ROOT::Flags_CXX IMPORTED INTERFACE)

# Fix for ROOT_CXX_FLAGS not actually being a CMake list
separate_arguments(ROOT_CXX_FLAGS)
set_property(TARGET ROOT::Flags_CXX APPEND PROPERTY
    INTERFACE_COMPILE_OPTIONS ${ROOT_CXX_FLAGS})

# Add definitions
separate_arguments(ROOT_DEFINITIONS)
foreach(_flag ${ROOT_EXE_LINKER_FLAG_LIST})
    # Remove -D or /D if present
    string(REGEX REPLACE [=[^[-//]D]=] "" _flag ${_flag})
    set_property(TARGET ROOT::Flags APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${_flag})
endforeach()

# This also fixes a bug in the linker flags
string(REPLACE "-L " "-L" ROOT_EXE_LINKER_FLAGS "${ROOT_EXE_LINKER_FLAGS}")
separate_arguments(ROOT_EXE_LINKER_FLAGS)
set_property(TARGET ROOT::Flags_CXX APPEND PROPERTY
    INTERFACE_LINK_LIBRARIES ${ROOT_EXE_LINKER_FLAGS})

################################################################################
## Find JPetFramework
find_package(JPetFramework CONFIG REQUIRED)
if(JPetFramework_FOUND)
  message(STATUS "JPetFramework (version ${JPetFramework_VERSION}) was found using JPetFrameworkConfig")
endif()

################################################################################
## Find BOOST LIBs

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.50 REQUIRED 
             COMPONENTS filesystem
                        program_options
                        regex
                        system
                        unit_test_framework
                        log_setup
                        log
                        )

if(NOT TARGET Boost::filesystem)
    add_library(Boost::filesystem IMPORTED INTERFACE)
    set_property(TARGET Boost::filesystem PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
    set_property(TARGET Boost::filesystem PROPERTY
        INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})
endif()

################################################################################
## Find Threads package
#
find_package(Threads REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)

add_executable(EventDisplay.exe ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(EventDisplay.exe eventDisplay JPetFramework::JPetFramework 
                                                    Unpacker2::Unpacker2 
                                                    Boost::filesystem 
                                                    ROOT::Rint 
                                                    ROOT::Gui 
                                                    ROOT::Geom 
                                                    ROOT::Graf3d
                                                    Threads::Threads)

if(NOT DOWNLOAD_DATA_HAPPENED_EVENT_DISPLAY)
  file(DOWNLOAD "http://sphinx.if.uj.edu.pl/framework/Examples/LargeBarrelAnalysis/large_barrel.json" ${CMAKE_CURRENT_BINARY_DIR}/large_barrel.json)
  set(DOWNLOAD_DATA_HAPPENED_EVENT_DISPLAY TRUE CACHE BOOL "Has the download happened?" FORCE)
endif()

message(STATUS "PREFIX: ${CMAKE_PREFIX_PATH}")
