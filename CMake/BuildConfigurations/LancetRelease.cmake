message(STATUS "Configuring Lancet Build")

#include(${CMAKE_CURRENT_LIST_DIR}/Default.cmake)

set(MITK_CONFIG_PACKAGES ${MITK_CONFIG_PACKAGES}
  
  OpenMesh
  Qt5
  BLUEBERRY
)

set(MITK_USE_OpenCV ON CACHE BOOL "MITK Use OpenCV Library" FORCE)
set(MITK_USE_OpenIGTLink ON CACHE BOOL "MITK Use OpenIGTLink Library" FORCE)

set(MITK_CONFIG_PLUGINS ${MITK_CONFIG_PLUGINS}
  # org.mitk.gui.qt.multilabelsegmentation
  # org.mitk.matchpoint.core.helper
  # org.mitk.gui.qt.matchpoint.algorithm.browser
  # org.mitk.gui.qt.matchpoint.algorithm.control
  # org.mitk.gui.qt.matchpoint.mapper
  # org.mitk.gui.qt.matchpoint.framereg
  # org.mitk.gui.qt.matchpoint.visualizer
  # org.mitk.gui.qt.matchpoint.evaluator
  # org.mitk.gui.qt.matchpoint.manipulator
  org.mitk.gui.qt.igttracking
  org.mitk.gui.qt.openigtlink
  # org.mitk.gui.qt.moviemaker
  # org.mitk.planarfigure
  org.mitk.gui.qt.datamanager
  org.mitk.gui.qt.stdmultiwidgeteditor
  org.mitk.gui.qt.dicombrowser
  org.mitk.gui.qt.imagenavigator
  org.mitk.gui.qt.measurementtoolbox
  org.mitk.gui.qt.properties
  org.mitk.gui.qt.segmentation
  org.mitk.gui.qt.volumevisualization
  org.mitk.planarfigure
#  org.mitk.gui.qt.moviemaker
  org.mitk.gui.qt.pointsetinteraction
  org.mitk.gui.qt.remeshing
  org.mitk.gui.qt.viewnavigator
  org.mitk.gui.qt.imagecropper
  org.mitk.gui.qt.basicimageprocessing
)

if(NOT MITK_USE_SUPERBUILD)
  set(BUILD_CoreCmdApps ON CACHE BOOL "" FORCE)
endif()

set(MITK_VTK_DEBUG_LEAKS OFF CACHE BOOL "Enable VTK Debug Leaks" FORCE)

find_package(Doxygen REQUIRED)

# Ensure that the in-application help can be build
set(BLUEBERRY_QT_HELP_REQUIRED ON CACHE BOOL "Required Qt help documentation in plug-ins" FORCE)
