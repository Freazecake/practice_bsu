QT += testlib gui
QT -= widgets

TARGET = WarehouseModelTests
TEMPLATE = app
CONFIG += c++17 console testcase
CONFIG -= app_bundle

SOURCES += \
    tst_warehousemodel.cpp \
    ../warehousemodel.cpp

HEADERS += \
    tst_warehousemodel.h \
    ../warehousemodel.h

INCLUDEPATH += ..
