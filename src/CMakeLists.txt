cmake_minimum_required(VERSION 3.18)

#Directories for the compiler
link_directories ( ${LIBGLADE_LIBRARY_DIRS} ${GTKMM_LIBRARY_DIRS}  ${GTHREAD2_LIBRARY_DIRS} ${SDL2_LIBRARY_DIRS} ${ASAP_LIB})
include_directories (${LIBGLADE_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS}  ${GTHREAD2_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${ASAP_INCLUDE} )

#Main mmsap module
add_executable( mmsap
  mmsap.cc
  alsaplayer.cc
  asma.cc
  columnmodel.cc
  gui.cc
  playlist.cc
  preferences.cc
  uidata.asm
)

#Compiler
#target_compile_options (mmsap PUBLIC -DGDK_PIXBUF_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED)

#Linker
target_link_libraries(mmsap
 ${GTKMM_LIBRARIES} ${LIBGLADE_LIBRARIES} ${ASAP_LIB} ${SDL2_LIBRARIES}
 ${GTHREAD2_LIBRARIES}
)

#Files to install
install(TARGETS mmsap RUNTIME DESTINATION bin)
install(FILES gladegui/mmsap.ui DESTINATION share/mmsap)