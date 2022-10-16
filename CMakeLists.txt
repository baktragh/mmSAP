#Minimum version of CMAKE
cmake_minimum_required(VERSION 3.18)

#Project
project(mmsap)

#Look for pkg-config
find_package(PkgConfig)

#Check whether the required GTKmm libraries are present
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0>=3.0)
pkg_check_modules(GTHREAD2 REQUIRED gthread-2.0>=2.12.0)


#Check whether the SDL2 library is present
pkg_check_modules(SDL2 REQUIRED sdl2>=2.0)

#Check for ASAP - library
find_library (ASAP_LIB asap REQUIRED)

#Check for ASAP - include file
find_path (ASAP_INCLUDE asap.h REQUIRED) 

#Check for xxd utility
find_program (PROGRAM_XXD xxd REQUIRED)

#Variables for configure file
set (MMSAP_PREFIX ${CMAKE_INSTALL_PREFIX})


#Prepare configuration file
configure_file (
  "${PROJECT_SOURCE_DIR}/src/mmsap_config.h.in"
  "${PROJECT_SOURCE_DIR}/src/mmsap_config.h"
)

#Add subdirectory with sources
add_subdirectory(src) 