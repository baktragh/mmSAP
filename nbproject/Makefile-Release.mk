#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++.exe
CXX=g++.exe
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/alsaplayer.o \
	${OBJECTDIR}/asma.o \
	${OBJECTDIR}/columnmodel.o \
	${OBJECTDIR}/gui.o \
	${OBJECTDIR}/mmsap2.o \
	${OBJECTDIR}/playlist.o \
	${OBJECTDIR}/preferences.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32 -Wl,-subsystem,windows
CXXFLAGS=-m32 -Wl,-subsystem,windows

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/C/appbuild/msys2/mingw32/lib -L/C/appbuild/msys2/usr/local/lib -lmingw32 -lSDL2Main -lSDL2 -lasap `pkg-config --libs gtkmm-2.4` `pkg-config --libs gthread-2.0`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /c/michael/winprog/mmsap-win32/bin/mmsap.exe

/c/michael/winprog/mmsap-win32/bin/mmsap.exe: ${OBJECTFILES}
	${MKDIR} -p /c/michael/winprog/mmsap-win32/bin
	g++.exe -o c:/michael/winprog/mmsap-win32/bin/mmsap ${OBJECTFILES} ${LDLIBSOPTIONS} -mwindows -s

${OBJECTDIR}/alsaplayer.o: alsaplayer.cc nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -I/C/appbuild/msys2/mingw32/include -I/C/appbuild/msys2/usr/local/include `pkg-config --cflags gtkmm-2.4` `pkg-config --cflags gthread-2.0`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/alsaplayer.o alsaplayer.cc

${OBJECTDIR}/asma.o: asma.cc nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -I/C/appbuild/msys2/mingw32/include -I/C/appbuild/msys2/usr/local/include `pkg-config --cflags gtkmm-2.4` `pkg-config --cflags gthread-2.0`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/asma.o asma.cc

${OBJECTDIR}/columnmodel.o: columnmodel.cc nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -I/C/appbuild/msys2/mingw32/include -I/C/appbuild/msys2/usr/local/include `pkg-config --cflags gtkmm-2.4` `pkg-config --cflags gthread-2.0`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/columnmodel.o columnmodel.cc

${OBJECTDIR}/gui.o: gui.cc nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -I/C/appbuild/msys2/mingw32/include -I/C/appbuild/msys2/usr/local/include `pkg-config --cflags gtkmm-2.4` `pkg-config --cflags gthread-2.0`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/gui.o gui.cc

${OBJECTDIR}/mmsap2.o: mmsap2.cc nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -I/C/appbuild/msys2/mingw32/include -I/C/appbuild/msys2/usr/local/include `pkg-config --cflags gtkmm-2.4` `pkg-config --cflags gthread-2.0`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mmsap2.o mmsap2.cc

${OBJECTDIR}/playlist.o: playlist.cc nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -I/C/appbuild/msys2/mingw32/include -I/C/appbuild/msys2/usr/local/include `pkg-config --cflags gtkmm-2.4` `pkg-config --cflags gthread-2.0`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/playlist.o playlist.cc

${OBJECTDIR}/preferences.o: preferences.cc nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -s -I/C/appbuild/msys2/mingw32/include -I/C/appbuild/msys2/usr/local/include `pkg-config --cflags gtkmm-2.4` `pkg-config --cflags gthread-2.0`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/preferences.o preferences.cc

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
