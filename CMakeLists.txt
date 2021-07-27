#Minimum version of CMAKE
cmake_minimum_required(VERSION 2.6)

#Project
project(mmsap)

#Look for pkg-config
find_package(PkgConfig)

#Check whether Gtkmm,libglade-2.0 and dbus-glib are present
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0>=3.0)
pkg_check_modules(GTHREAD2 REQUIRED gthread-2.0>=2.12.0)


#Check for ASAP
find_library (ASAP_LIB asap)
if ("${ASAP_LIB}" STREQUAL "ASAP-NOTFOUND")
    message(FATAL_ERROR "ASAP library not found")
endif ("${ASAP_LIB}" STREQUAL "ASAP-NOTFOUND")

#Check for ALSA
find_library (ALSA_LIB asound)
if ("${ALSA_LIB}" STREQUAL "ALSA-NOTFOUND")
    message(FATAL_ERROR "ALSA library not found")
endif ("${ALSA_LIB}" STREQUAL "ALSA-NOTFOUND")

#Variables for configure file
set (MMSAP_PREFIX ${CMAKE_INSTALL_PREFIX})

#Prepare configuration file
configure_file (
  "${PROJECT_SOURCE_DIR}/src/mmsap_config.h.in"
  "${PROJECT_SOURCE_DIR}/src/mmsap_config.h"
)

#Add subdirectory with sources
add_subdirectory(src)
 
