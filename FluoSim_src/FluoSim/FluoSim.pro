
include($$PWD/../../FluoSim_withLibraries.pri)

QT += \
    core \
    gui \
    widgets \
    opengl

CONFIG += \
#    CONSOLE \
    C++11

TEMPLATE = app
TARGET = FluoSim
equals(isUsingStaticLib, true) {
    DEFINES += GPUTOOLS_LIBRARYSTATIC #needed to link to the static library
    DEFINES += TOOLBOX_LIBRARYSTATIC #needed to link to the static library
#    DEFINES += CELLENGINE_LIBRARYSTATIC #needed to link to the static library
    message("Using Staticlib.")
}
else {
    message("Using dll.")
}
RC_FILE = logo.rc

GPUTOOLS_LIBRARY_PATH = $$PWD/../gpuTools
TOOLBOX_LIBRARY_PATH = $$PWD/../toolBox
CELLENGINE_LIBRARY_PATH = $$PWD/../cellEngine
FLUOSIM_DEPENDENCIES_PATH = $$PWD/../../FluoSim_dependencies
DESTDIR = $$PWD/FluoSim_build/release
OBJECTS_DIR = $$PWD/FluoSim_build/release
MOC_DIR = $$PWD/FluoSim_build/release
INSTALL_DIR = $$PWD/../../FluoSim_build/release #where to put the executable after compilation

INCLUDEPATH += \
\
    $$TOOLBOX_LIBRARY_PATH/ \
        $$TOOLBOX_LIBRARY_PATH/toolBox_src \
    $$GPUTOOLS_LIBRARY_PATH/ \
        $$GPUTOOLS_LIBRARY_PATH/gpuTools_src \
    $$CELLENGINE_LIBRARY_PATH/ \
        $$CELLENGINE_LIBRARY_PATH/cellEngine_src \
    $$FLUOSIM_DEPENDENCIES_PATH/openGL/include \
    $$FLUOSIM_DEPENDENCIES_PATH/glm \
    $$FLUOSIM_DEPENDENCIES_PATH/libtiff/include \
    $$FLUOSIM_DEPENDENCIES_PATH/glew/include \
    $$FLUOSIM_DEPENDENCIES_PATH/TinyTiff/include \
    $$FLUOSIM_DEPENDENCIES_PATH/lmfit/include

LIBS += \
\
    -L$$FLUOSIM_DEPENDENCIES_PATH/glew/bin \
    -L$$FLUOSIM_DEPENDENCIES_PATH/openGL \
    -L$$FLUOSIM_DEPENDENCIES_PATH/libtiff/bin \
    -L$$FLUOSIM_DEPENDENCIES_PATH/TinyTiff/bin \
    -L$$FLUOSIM_DEPENDENCIES_PATH/lmfit/bin \
    -L$$GPUTOOLS_LIBRARY_PATH/gpuTools_build/release \
    -L$$TOOLBOX_LIBRARY_PATH/toolBox_build/release \
    -L$$CELLENGINE_LIBRARY_PATH/cellEngine_build/release \
\
        -lglew32 \
        -lopengl32 \
        -llibtiff3 \
        -lgpuTools_library \
        -ltoolBox_library \
        -lcellEngine_library \
        -ltinytiff \
        -llmfit

SOURCES += \
\
    FluoSim_src/FluoSimControl.cpp \
    FluoSim_src/FluoSimModel.cpp \
    FluoSim_src/FluoSimView.cpp \
    FluoSim_src/main.cpp \
    FluoSim_src/TracePlayerControl.cpp \
    FluoSim_src/TracePlayerModel.cpp \
    FluoSim_src/TracePlayerView.cpp

HEADERS += \
\
    FluoSim_src/FluoSim.h \
    FluoSim_src/TracePlayer.h

win32 {
    DESTDIR ~= s,/,\\,g
    INSTALL_DIR ~= s,/,\\,g
    message($$DESTDIR)
    message($$INSTALL_DIR)
}

QMAKE_POST_LINK += xcopy $$quote($$DESTDIR\\$${TARGET}.exe) $$quote($$INSTALL_DIR\\$${TARGET}.exe*) /Y




