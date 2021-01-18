
TEMPLATE = lib
TARGET = lmfit
CONFIG += staticlib


SOURCES += \
    lmfit_src/lmcurve.c \
    lmfit_src/lmmin.c \
    lmfit_src/lmcurve_tyd.c

HEADERS += \
    lmfit_src/lmcurve.h \
    lmfit_src/lmcurve_tyd.h \
    lmfit_src/lmstruct.h \
    lmfit_src/lmmin.h
