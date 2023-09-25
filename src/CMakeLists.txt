cmake_minimum_required(VERSION 3.18)

#Directories for the compiler
link_directories (  ${GTKMM_LIBRARY_DIRS}  ${GTHREAD2_LIBRARY_DIRS} ${SDL2_LIBRARY_DIRS} ${ASAP_LIB})
include_directories ( ${GTKMM_INCLUDE_DIRS}  ${GTHREAD2_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${ASAP_INCLUDE} )

add_custom_command (
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/uidata.h
  COMMAND  cat ${CMAKE_CURRENT_SOURCE_DIR}/../gladegui/mmsap.ui | xxd -i  > ${CMAKE_CURRENT_SOURCE_DIR}/uidata.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../gladegui/mmsap.ui
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

#Main mmsap module
add_executable( mmsap
  mmsap.cc
  sdlplayer.cc
  asma.cc
  columnmodel.cc
  gui.cc
  playlist.cc
  preferences.cc
  uidata.c
  uidata.h
)

#Linker
target_link_libraries(mmsap
 ${GTKMM_LIBRARIES}  ${ASAP_LIB} ${SDL2_LIBRARIES} ${GTHREAD2_LIBRARIES}
)



#Compiler option
if (MSYS)
target_compile_options(mmsap PRIVATE -mwindows)
target_link_options(mmsap PRIVATE -mwindows)
endif (MSYS)

#Files to install
install(TARGETS mmsap RUNTIME DESTINATION bin)
