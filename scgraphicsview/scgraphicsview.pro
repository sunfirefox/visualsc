#-------------------------------------------------
#
#
#-------------------------------------------------


include (../visualsc.pri)

QT       += core gui opengl

TARGET = scgraphicsview
TEMPLATE = lib

scgraphicsview.depends = scdatamodel

$QMAKE_CLEAN += $$TARGET

DEFINES += SCGRAPHICSVIEW_LIBRARY


INCLUDEPATH += ../scdatamodel
win32:LIBS += $${LIB_PATH}/scdatamodel.lib
unix:LIBS += -L$${LIB_PATH} -lscdatamodel


SOURCES += scgraphicsview.cpp \ 
    stateboxgraphic.cpp \
    selectablelinesegmentgraphic.cpp \
    cornergrabber.cpp \
    selectableboxgraphic.cpp \
    selectabletextblock.cpp \
    textformattoolbar.cpp \
    texteditbox.cpp \
    buttongraphic.cpp \
    maskedtextedit.cpp \
    transitiongraphic.cpp \
    arrowheadgraphic.cpp \
    keycontroller.cpp \
    elbowgrabber.cpp \
    mousecontroller.cpp \
    linesegmentgraphic.cpp \
    fixedtextblock.cpp \
    customgraphicsview.cpp \
    togglebutton.cpp \
    customgraphicsscene.cpp


HEADERS += scgraphicsview.h\
        scgraphicsview_global.h \ 
    stateboxgraphic.h \
    selectablelinesegmentgraphic.h \
    cornergrabber.h \
    selectableboxgraphic.h \
    selectabletextblock.h \
    textformattoolbar.h \
    texteditbox.h \
    buttongraphic.h \
    maskedtextedit.h \
    transitiongraphic.h \
    arrowheadgraphic.h \
    keycontroller.h \
    elbowgrabber.h \
    mousecontroller.h \
    linesegmentgraphic.h \
    fixedtextblock.h \
    customgraphicsview.h \
    togglebutton.h \
    customgraphicsscene.h


OTHER_FILES += \
    COPYING.txt

RESOURCES += \
    resources/scgraphicsview.qrc
