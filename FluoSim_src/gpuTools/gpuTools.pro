include($$PWD/../../FluoSim_withLibraries.pri)

CONFIG += C++11
TARGET = gpuTools_library
TEMPLATE = lib
equals(isUsingStaticLib, true) {
    CONFIG += staticlib
    DEFINES += GPUTOOLS_LIBRARYSTATIC
    message("Using Staticlib.")
}
else {
    CONFIG += dll
    DFINES += GPUTOOLS_LIBRARYSHARED
    message("Using dll.")
}

SOURCES += \
    gpuTools_src/gl/gEnum.cpp \
    gpuTools_src/gl/gProgram.cpp \
    gpuTools_src/gl/gShader.cpp \
    gpuTools_src/gl/gTexture.cpp \
    gpuTools_src/gl/gToolBox.cpp \
    gpuTools_src/gl/gVector.cpp \
    gpuTools_src/other/string_manipulation.cpp \
    gpuTools_src/other/gpu_tiff_manipulation.cpp \
    gpuTools_src/other/gpu_otherFunctions.cpp

HEADERS +=\
    gpuTools_src/gpuTools_library_global.h \
    gpuTools_src/gl/gEnum.h \
    gpuTools_src/gl/gProgram.h \
    gpuTools_src/gl/gTexture.h \
    gpuTools_src/gl/gToolBox.h \
    gpuTools_src/gl/gVector.h \
    gpuTools_src/gl/gMultiVector.h \
    gpuTools_src/other/gpu_string_manipulation.h \
    gpuTools_src/other/gpu_myFileHandler.h \
    gpuTools_src/other/gpu_tiff_manipulation.h \
    gpuTools_src/other/gpu_otherFunctions.h

FLUOSIM_DEPENDENCIES_PATH = $$PWD/../../FluoSim_dependencies
DESTDIR = $$PWD/gpuTools_build/release
OBJECTS_DIR = $$PWD/gpuTools_build/release
MOC_DIR = $$PWD/gpuTools_build/release
INSTALL_DIR = $$PWD/../../FluoSim_build/release #where to put the dll after compilation

INCLUDEPATH += \ #LIBRARIES
    $$FLUOSIM_DEPENDENCIES_PATH/openGL/include \
    $$FLUOSIM_DEPENDENCIES_PATH/glm \
    $$FLUOSIM_DEPENDENCIES_PATH/glew/include \
    $$FLUOSIM_DEPENDENCIES_PATH/libtiff/include

INCLUDEPATH += \ #LOCAL HEADERS
    ./gpuTools_src

message($$FLUOSIM_DEPENDENCIES_PATH)

LIBS += \
    -L$$FLUOSIM_DEPENDENCIES_PATH/glew/bin \
    -L$$FLUOSIM_DEPENDENCIES_PATH/openGL \
    -L$$FLUOSIM_DEPENDENCIES_PATH/libtiff/bin \
\
    -lopengl32 \
    -lglew32 \
    -llibtiff3

win32 {
    DESTDIR ~= s,/,\\,g
    INSTALL_DIR ~= s,/,\\,g
    message($$DESTDIR)
    message($$INSTALL_DIR)
}

#if a dll is produced, it is exported in the FluoSim_build directory
!equals(isUsingStaticLib, true) : QMAKE_POST_LINK += xcopy $$quote($$DESTDIR\\$${TARGET}.dll) $$quote($$INSTALL_DIR\\$${TARGET}.dll*) /Y

