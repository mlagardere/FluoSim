
TEMPLATE = subdirs

SUBDIRS = \
        FluoSim \
        cellEngine \
        gpuTools \
        toolBox

FluoSim.subdir = FluoSim_src/FluoSim
cellEngine.subdir = FluoSim_src/cellEngine
gpuTools.subdir = FluoSim_src/gpuTools
toolBox.subdir = FluoSim_src/toolBox

toolBox.depends = gpuTools
cellEngine.depends = toolBox
FluoSim.depends = cellEngine



