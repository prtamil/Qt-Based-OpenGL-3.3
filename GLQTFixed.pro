#-------------------------------------------------
#
# Project created by QtCreator 2012-01-01T02:24:32
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = GLQTFixed
TEMPLATE = app

LIBS += -lGLEW -lGLU

SOURCES += main.cpp \
    mainwindow.cpp

HEADERS  += \
    mainwindow.h

OTHER_FILES += \
    fragment.glsl \
    vertex.glsl
