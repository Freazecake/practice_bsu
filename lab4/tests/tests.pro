QT += testlib gui core
QT -= widgets

TARGET = VectorEditorTests
TEMPLATE = app
CONFIG += c++17 console testcase
CONFIG -= app_bundle

SOURCES += \
    tst_scene.cpp \
    ../scene.cpp \
    ../shape.cpp \
    ../shapes.cpp

HEADERS += \
    tst_scene.h \
    ../scene.h \
    ../shape.h \
    ../shapes.h

INCLUDEPATH += ..
