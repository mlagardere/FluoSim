
include($$PWD/../../FluoSim_withLibraries.pri)

CONFIG += \
    CONSOLE \
    C++11

QT += \
    core \
    gui \
    widgets \
    opengl

QMAKE_CXXFLAGS += -std=c++11
TEMPLATE = lib
TARGET = toolBox_library
equals(isUsingStaticLib, true) {
    CONFIG += staticlib
    DEFINES += GPUTOOLS_LIBRARYSTATIC #needed to link to the static library
    DEFINES += TOOLBOX_LIBRARYSTATIC
    message("Using Staticlib.")
}
else {
    CONFIG += dll
    DEFINES += TOOLBOX_LIBRARYSHARED
    message("Using dll.")
}


GPUTOOLS_LIBRARY_PATH = $$PWD/../gpuTools/
FLUOSIM_DEPENDENCIES_PATH = $$PWD/../../FluoSim_dependencies
DESTDIR = $$PWD/toolBox_build/release
OBJECTS_DIR = $$PWD/toolBox_build/release
MOC_DIR = $$PWD/toolBox_build/release
INSTALL_DIR = $$PWD/../../FluoSim_build/release #where to put the dll after compilation

INCLUDEPATH += \
    toolBox_src \
    $$FLUOSIM_DEPENDENCIES_PATH/openGL/include \
    $$FLUOSIM_DEPENDENCIES_PATH/glm \
    $$FLUOSIM_DEPENDENCIES_PATH/glew/include \
    $$FLUOSIM_DEPENDENCIES_PATH/libtiff/include \
    $$GPUTOOLS_LIBRARY_PATH/gpuTools_src \
    $$FLUOSIM_DEPENDENCIES_PATH/TinyTiff\include

LIBS += \
    -L$$FLUOSIM_DEPENDENCIES_PATH/openGL \
    -L$$FLUOSIM_DEPENDENCIES_PATH/glew/bin \
    -L$$FLUOSIM_DEPENDENCIES_PATH/libtiff/bin \
    -L$$GPUTOOLS_LIBRARY_PATH/gpuTools_build/release \
    -L$$FLUOSIM_DEPENDENCIES_PATH/TinyTiff/bin \
\
    -lglew32 \
    -lopengl32 \
    -llibtiff3 \
    -lgpuTools_library \
    -ltinytiff

SOURCES += \
    toolBox_src/developmentTools/myClock.cpp \
    toolBox_src/fileAndstring_manipulation/file_manipulation.cpp \
    toolBox_src/fileAndstring_manipulation/string_manipulation.cpp \
    toolBox_src/geometry/myGeomtricObject.cpp \
    toolBox_src/glGUI/glObjects/textRendering/myFont.cpp \
    toolBox_src/glGUI/glObjects/textRendering/myWord.cpp \
    toolBox_src/glGUI/glObjects/myDrawableObject.cpp \
    toolBox_src/glGUI/glObjects/myEditableObject.cpp \
    toolBox_src/glGUI/glObjects/myGLGeomtricObject.cpp \
    toolBox_src/glGUI/glObjects/myGLImage.cpp \
    toolBox_src/glGUI/glObjects/myGLImageObjects.cpp \
    toolBox_src/glGUI/glObjects/myGLObject.cpp \
    toolBox_src/glGUI/glObjects/myGLTiffMovieObject.cpp \
    toolBox_src/glGUI/glObjects/myRenderableObject.cpp \
    toolBox_src/glGUI/glObjects/myRenderingTargetObject.cpp \
    toolBox_src/glGUI/glObjects/mySelectionableObject.cpp \
    toolBox_src/glGUI/glScreenObjects/myGLScreenGeometricObjects.cpp \
    toolBox_src/glGUI/glScreenObjects/myGLScreenImageObjects.cpp \
    toolBox_src/glGUI/glScreenObjects/myGLScreenLoopedGeometry.cpp \
    toolBox_src/glGUI/glScreenObjects/myGLScreenMultiLineGeometry.cpp \
    toolBox_src/glGUI/glScreenObjects/myGLScreenProceduralImage.cpp \
    toolBox_src/glGUI/glWidgets/myGLScreen/myGLScreenModel.cpp \
    toolBox_src/glGUI/glWidgets/myGLScreen/myGLScreenView.cpp \
    toolBox_src/glGUI/glWidgets/myGLScreen/myScreenControl.cpp \
    toolBox_src/glGUI/glWidgets/myGLWidget.cpp \
    toolBox_src/imageTools/image_colorisation/myLUT.cpp \
    toolBox_src/imageTools/image_format/tiff_format.cpp \
    toolBox_src/imageTools/TIFFMovie/myTiffMovie.cpp \
    toolBox_src/myQtWidgets/myDockwidget.cpp \
    toolBox_src/myQtWidgets/myDropMenu.cpp \
    toolBox_src/myQtWidgets/myMainWindow.cpp \
    toolBox_src/myQtWidgets/myProgressWidget.cpp \
    toolBox_src/myQtWidgets/mySpinBoxedSlider.cpp \
    toolBox_src/myQtWidgets/myDoubleSpinBoxedSlider.cpp \
    toolBox_src/myQtWidgets/mytablewidget.cpp \
    toolBox_src/myQtWidgets/myComboBox.cpp \
    toolBox_src/otherFunctions/otherFunctions.cpp

HEADERS += \
    toolBox_src/containers/myMultiVector.h \
    toolBox_src/developmentTools/myClock.h \
    toolBox_src/fileAndstring_manipulation/file_manipulation.h \
    toolBox_src/fileAndstring_manipulation/string_manipulation.h \
    toolBox_src/geometry/myGeomtricObject.h \
    toolBox_src/glGUI/glObjects/textRendering/myFont.h \
    toolBox_src/glGUI/glObjects/textRendering/myWord.h \
    toolBox_src/glGUI/glObjects/myGLGeomtricObject.h \
    toolBox_src/glGUI/glObjects/myGLImageObjects.h \
    toolBox_src/glGUI/glObjects/myGLObject.h \
    toolBox_src/glGUI/glObjects/myGLTiffMovieObject.h \
    toolBox_src/glGUI/glScreenObjects/myGLScreenGeometricObjects.h \
    toolBox_src/glGUI/glScreenObjects/myGLScreenImageObjects.h \
    toolBox_src/glGUI/glScreenObjects/myGLScreenProceduralImage.h \
    toolBox_src/glGUI/glScreenObjects/myTiffMovie.h \
    toolBox_src/glGUI/glWidgets/myGLScreen/myGLScreen.h \
    toolBox_src/glGUI/glWidgets/myGLWidget.h \
    toolBox_src/imageTools/image_colorisation/myLUT.h \
    toolBox_src/imageTools/image_format/tiff_format.h \
    toolBox_src/imageTools/TIFFMovie/myTiffMovie.h \
    toolBox_src/myQtWidgets/myDockwidget.h \
    toolBox_src/myQtWidgets/myDropMenu.h \
    toolBox_src/myQtWidgets/myMainWindow.h \
    toolBox_src/myQtWidgets/myProgressWidget.h \
    toolBox_src/toolbox_library_global.h \
    toolBox_src/myQtWidgets/mySpinBoxedSlider.h \
    toolBox_src/myQtWidgets/myDoubleSpinBoxedSlider.h \
    toolBox_src/myQtWidgets/mytablewidget.h \
    toolBox_src/myQtWidgets/myComboBox.h \
    toolBox_src/otherFunctions/otherFunctions.h

win32 {
    DESTDIR ~= s,/,\\,g
    INSTALL_DIR ~= s,/,\\,g
    message($$DESTDIR)
    message($$INSTALL_DIR)
}

#if a shared library is created, it is exported in the FluoSim_build directory
!equals(isUsingStaticLib, true) QMAKE_POST_LINK += xcopy $$quote($$DESTDIR\\$${TARGET}.dll) $$quote($$INSTALL_DIR\\$${TARGET}.dll*) /Y


