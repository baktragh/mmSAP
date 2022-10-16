Introduction 
===============
   mmSAP s SAP (Slight atari player) player based on ASAP library that uses SDL2 for audio output
   and has GTK 3 based GUI

   SAP is a special file format designed to store POKEY-based Atari XL/XE music.
   The biggest collection of such music is ASMA (Atari SAP Music Archive) that
   can be found at: http://asma.atari.org.

   ASAP library also allows you to load RMT,TMC,TM2,MPT,CMC files directly. For fresh
   information about supported formats, refer to the ASAP project.

   mmSAP's development has been revived in late 2022 with unified code base for Windows and Linux. The revival was inspired by a similar revival of the ASMA archive.

Features
===========

   * Based on the ASAP library (http://asap.sourceforge.net/) that provides precise and high fidelity POKEY emulation
   * Easy to use GTK 3 based user interface
   * Fully featured playlist
   * Configurable POKEY visualization
   * Good support for ASMA STIL database
   * Four repeat modes (no repeat, repeat subsong,repeat tune,repeat playlist)
   * Seeking inside subsongs that have TIME tag
   * Single instance application (using GTK 3 features)
   * Drag and drop support
   * 'Browse and play' file chooser
   * Audio output through SDL2
   * Possibility to mute selected POKEY channels

Operating systems supported
==============================
Microsoft Windows, GNU/Linux
   
Dependencies
===============
   * gtkmm 3.24, GTK 3.24 GUI libraries
   * SDL2 Multimedia library
   * ASAP (Download from http://asap.sourceforge.net/. Latest release is recommended.)
   * pkg-config tool
   * CMake build tool
   * GCC C++ Compiler
   * xxd utility

Compiling and installing under GNU/Linux
========================================
   * Install all the dependencies
   * Change working directory to the directory with mmSAP sources and invoke following commands: 
     
         cmake .
         make
         sudo make install
   
Compiling and installing under Microsoft Windows
================================================
In general, it is recommended to use the available binary packages.c

   * Install MSYS2
   * Install all dependencies
   * Change working directory to the directory with mmSAP sources and invoke the following commands
   
         cmake -G "MSYS Makefiles" .
         make
         sudo make install
         
         
