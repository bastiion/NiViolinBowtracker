cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(BowTracker)
 
## Add current Source Dir
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
## Add folder /include
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
###############################################################################
# Find OpenNI
#
# This sets the following variables:
# OPENNI_FOUND - True if OPENNI was found.
# OPENNI_INCLUDE_DIRS - Directories containing the OPENNI include files.
# OPENNI_LIBRARIES - Libraries needed to use OPENNI.
# OPENNI_DEFINITIONS - Compiler flags for OPENNI.
 
find_package(PkgConfig)
if(${CMAKE_VERSION} VERSION_LESS 2.8.2)
  pkg_check_modules(PC_OPENNI openni-dev)
else()
  pkg_check_modules(PC_OPENNI QUIET openni-dev)
endif()

SET(QMIDI_DIR "thirdparty/QMidi/src/")
SET(QMIDI_SRC   ${QMIDI_DIR}/QMidiFile.cpp 
                ${QMIDI_DIR}/QMidiOut.cpp 
                ${QMIDI_DIR}/OS/QMidi_ALSA.cpp 
                )
include_directories(${QMIDI_DIR} src)

SET(QT_USE_QTOPENGL TRUE)
SET(QT_MIN_VERSION "4.6.0" COMPONENTS QtCore QtGui QtXml QtNetwork QtOpenGL)
FIND_PACKAGE(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package( OpenGL REQUIRED )
include_directories(${OPENGL_INCLUDE_DIR})

FIND_PACKAGE( OpenCV REQUIRED )
FIND_PACKAGE( ALSA REQUIRED )
if(ALSA_FOUND)
include_directories(${ALSA_INCLUDE_DIRS})
link_directories(${ALSA_LIBRARY})
add_definitions(${ALSA_DEFINITIONS})
endif()

set(OPENNI_DEFINITIONS ${PC_OPENNI_CFLAGS_OTHER})

#add a hint so that it can find it without the pkg-config
find_path(OPENNI_INCLUDE_DIR XnStatus.h
      HINTS ${NESTK_ROOT_DIRS_HINTS} ${PC_OPENNI_INCLUDEDIR} ${PC_OPENNI_INCLUDE_DIRS} /usr/include/openni /usr/include/ni
      PATHS "$ENV{PROGRAMFILES}/OpenNI/Include" "$ENV{PROGRAMW6432}/OpenNI/Include"
      PATH_SUFFIXES openni)
#add a hint so that it can find it without the pkg-config
find_library(OPENNI_LIBRARY
         NAMES OpenNI64 OpenNI
         HINTS ${NESTK_ROOT_DIRS_HINTS} ${PC_OPENNI_LIBDIR} ${PC_OPENNI_LIBRARY_DIRS} /usr/lib
         PATHS "$ENV{PROGRAMFILES}/OpenNI/Lib${OPENNI_SUFFIX}" "$ENV{PROGRAMW6432}/OpenNI/Lib${OPENNI_SUFFIX}"
         PATH_SUFFIXES lib
)
find_library(NITE_LIBRARY
             NAMES XnVNite XnVNITE_1_3_1 XnVNITE_1_4_0 XnVNite_1_4_2 XnVNite_1_5_2
             HINTS ${NESTK_ROOT_DIRS_HINTS} ${PC_OPENNI_LIBDIR} ${PC_OPENNI_LIBRARY_DIRS} /usr/lib
             PATHS "$ENV{PROGRAMFILES}/PrimeSense/NITE/Lib${OPENNI_SUFFIX}" "$ENV{PROGRAMW6432}/PrimeSense/NITE/Lib${OPENNI_SUFFIX}"
             PATH_SUFFIXES lib
)
 
find_path(NITE_INCLUDE_DIR XnVSessionManager.h
          HINTS ${NESTK_ROOT_DIRS_HINTS} ${PC_OPENNI_INCLUDEDIR} ${PC_OPENNI_INCLUDE_DIRS} /usr/include/openni /usr/include/nite
          PATHS "$ENV{PROGRAMFILES}/PrimeSense/NITE/Include" "$ENV{PROGRAMW6432}/PrimeSense/NITE/Include"
          PATH_SUFFIXES openni)
 
set(OPENNI_INCLUDE_DIRS ${OPENNI_INCLUDE_DIR} ${NITE_INCLUDE_DIR})
if(APPLE)
  set(OPENNI_LIBRARIES ${OPENNI_LIBRARY} ${NITE_LIBRARY} usb)
else()
  set(OPENNI_LIBRARIES ${OPENNI_LIBRARY} ${NITE_LIBRARY})
endif()
 
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenNI DEFAULT_MSG
    OPENNI_LIBRARY OPENNI_INCLUDE_DIR)
 
mark_as_advanced(OPENNI_LIBRARY OPENNI_INCLUDE_DIR)
if(OPENNI_FOUND)
  include_directories(${OPENNI_INCLUDE_DIRS})
  link_directories(${OPENNI_LIBRARY})
  add_definitions(${OPENNI_DEFINITIONS})
  message(STATUS "OpenNI found (include: ${OPENNI_INCLUDE_DIR}, lib: ${OPENNI_LIBRARY})")
endif(OPENNI_FOUND)
 
################### ####################### PCL
 
#find_package(PCL 1.3 REQUIRED)
#include_directories(${PCL_INCLUDE_DIRS})
#link_directories(${PCL_LIBRARY_DIRS})
#add_definitions(${PCL_DEFINITIONS})
set(BOWTRACKER_SRCS
    src/main.cpp
    src/mainwindow.cpp
    src/qtglwidget.cpp
    src/imagehelper.cpp
    src/kinect.cpp
    src/houghDetection.cpp
    src/midisink.cpp
    ${QMIDI_SRC}
    )

set(BOWTRACKER_MOC_SRC
    src/mainwindow.h
    src/qtglwidget.h
    src/kinect.h
    )
 
QT4_WRAP_CPP(BOWTRACKER_MOC ${BOWTRACKER_MOC_SRC})
add_compile_options (-std=gnu++11)
add_executable (bowtracker ${BOWTRACKER_SRCS} ${BOWTRACKER_MOC}) 
target_link_libraries (bowtracker ${OPENNI_LIBRARY} ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${OpenCV_LIBS} ${ALSA_LIBRARIES})
