QT       += core gui widgets

TARGET = VectorEditor
TEMPLATE = app
CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    canvas.cpp \
    scene.cpp \
    shape.cpp \
    shapes.cpp

HEADERS += \
    mainwindow.h \
    canvas.h \
    scene.h \
    shape.h \
    shapes.h
