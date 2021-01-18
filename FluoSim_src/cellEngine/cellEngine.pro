include($$PWD/../../FluoSim_withLibraries.pri)

CONFIG += \
    C++11
    CONSOLE

QT += \
    core \
    gui \
    widgets \
    opengl

TARGET = cellEngine_library
TEMPLATE = lib
equals(isUsingStaticLib, true) {
#    CONFIG += staticlib
    CONFIG += dll #the cellEngine has to be a dll : static lib does not work...
    DEFINES += GPUTOOLS_LIBRARYSTATIC #needed to link to the static library
    DEFINES += TOOLBOX_LIBRARYSTATIC #needed to link to the static library
    DEFINES += CELLENGINE_LIBRARYSHARED
    message("Using Staticlib.")
}
else {
    CONFIG += dll
    DEFINES += CELLENGINE_LIBRARYSHARED
    message("Using dll.")
}

GPUTOOLS_LIBRARY_PATH = $$PWD/../gpuTools
TOOLBOX_LIBRARY_PATH = $$PWD/../toolBox
FLUOSIM_DEPENDENCIES_PATH = $$PWD/../../FluoSim_dependencies
DESTDIR = $$PWD/cellEngine_build/release
OBJECTS_DIR = $$PWD/cellEngine_build/release
MOC_DIR = $$PWD/cellEngine_build/release
INSTALL_DIR = $$PWD/../../FluoSim_build/release #where to put the dll after compilation

INCLUDEPATH += \
    cellEngine_src \
    $$FLUOSIM_DEPENDENCIES_PATH/openGL/include \
    $$FLUOSIM_DEPENDENCIES_PATH/glm \
    $$FLUOSIM_DEPENDENCIES_PATH/glew/include \
    $$FLUOSIM_DEPENDENCIES_PATH/libtiff/include \
    $$FLUOSIM_DEPENDENCIES_PATH/lmfit/include \
    $$GPUTOOLS_LIBRARY_PATH \
        $$GPUTOOLS_LIBRARY_PATH/gpuTools_src \
    $$TOOLBOX_LIBRARY_PATH \
        $$TOOLBOX_LIBRARY_PATH/toolBox_src \

LIBS += \
    -L$$FLUOSIM_DEPENDENCIES_PATH/openGL \
    -L$$FLUOSIM_DEPENDENCIES_PATH/glew/bin \
    -L$$FLUOSIM_DEPENDENCIES_PATH/libtiff/bin \
    -L$$FLUOSIM_DEPENDENCIES_PATH/lmfit/bin \
    -L$$GPUTOOLS_LIBRARY_PATH/gpuTools_build/release \
    -L$$TOOLBOX_LIBRARY_PATH/toolBox_build/release \
\
    -llibtiff3 \
    -lopengl32 \
    -lgpuTools_library \
    -ltoolBox_library \
    -lglew32 \
    -llmfit


SOURCES += \
    cellEngine_src/biologicalWorld/Region_gpu.cpp \
    cellEngine_src/biologicalWorld/BiologicalWorld.cpp \
    cellEngine_src/biologicalWorld/ChemicalSpecies.cpp \
    cellEngine_src/biologicalWorld/Fluorophore.cpp \
    cellEngine_src/biologicalWorld/FluorophoreSpecies.cpp \
    cellEngine_src/biologicalWorld/FrapHead.cpp \
    cellEngine_src/biologicalWorld/Particle.cpp \
    cellEngine_src/biologicalWorld/Probe.cpp \
    cellEngine_src/displayAndcontrol/Graphics/Axis.cpp \
    cellEngine_src/displayAndcontrol/Graphics/Graphic.cpp \
    cellEngine_src/displayAndcontrol/Screen.cpp \
    cellEngine_src/displayAndcontrol/ScreenHandler.cpp \
    cellEngine_src/Measure/FluoEvent.cpp \
    cellEngine_src/Measure/Signal.cpp \
    cellEngine_src/Measure/Trace.cpp \
    cellEngine_src/physicsEngine/DiffusionSubEngine.cpp \
    cellEngine_src/physicsEngine/RandomNumberGenerator.cpp

HEADERS +=\
    cellEngine_src/cellEngine_library_global.h \
    cellEngine_src/biologicalWorld/ChemicalSpecies.h \
    cellEngine_src/biologicalWorld/Region_gpu.h \
    cellEngine_src/biologicalWorld/BiologicalWorld.h \
    cellEngine_src/biologicalWorld/Fluorophore.h \
    cellEngine_src/biologicalWorld/FluorophoreSpecies.h \
    cellEngine_src/biologicalWorld/FrapHead.h \
    cellEngine_src/biologicalWorld/Particle.h \
    cellEngine_src/biologicalWorld/Probe.h \
    cellEngine_src/displayAndcontrol/Graphics/Axis.h \
    cellEngine_src/displayAndcontrol/Graphics/Graphic.h \
    cellEngine_src/displayAndcontrol/Screen.h \
    cellEngine_src/displayAndcontrol/ScreenHandler.h \
    cellEngine_src/Measure/FluoEvent.h \
    cellEngine_src/Measure/Signal.h \
    cellEngine_src/Measure/Trace.h \
    cellEngine_src/physicsEngine/DiffusionSubEngine.h \
    cellEngine_src/physicsEngine/RandomNumberGenerator.h

win32 {
    DESTDIR ~= s,/,\\,g
    INSTALL_DIR ~= s,/,\\,g
    message($$DESTDIR)
    message($$INSTALL_DIR)
}

QMAKE_POST_LINK += xcopy $$quote($$DESTDIR\\$${TARGET}.dll) $$quote($$INSTALL_DIR\\$${TARGET}.dll*) /Y





