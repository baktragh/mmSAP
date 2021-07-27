cmake_minimum_required(VERSION 2.6)

#Directories for the compiler
link_directories ( ${LIBGLADE_LIBRARY_DIRS} ${GTKMM_LIBRARY_DIRS}  ${GTHREAD2_LIBRARY_DIRS})
include_directories (${LIBGLADE_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS}  ${GTHREAD2_INCLUDE_DIRS})

#Main mmsap module
add_executable( mmsap
  mmsap.cc
  alsaplayer.cc
  asma.cc
  columnmodel.cc
  gui.cc
  playlist.cc
  preferences.cc
)

#Linker
target_link_libraries(mmsap
 ${GTKMM_LIBRARIES} ${LIBGLADE_LIBRARIES} ${ASAP_LIB} ${ALSA_LIB}
 ${GTHREAD2_LIBRARIES}
)

#Files to install
install(TARGETS mmsap RUNTIME DESTINATION bin)
install(FILES gladegui/mmsap.ui gladegui/mmsap_alt.ui DESTINATION share/mmsap)

