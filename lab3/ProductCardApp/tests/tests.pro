QT += testlib core
QT -= gui widgets

TARGET = ProductCardStoreTests
TEMPLATE = app
CONFIG += c++17 console testcase
CONFIG -= app_bundle

SOURCES += \
    tst_productcardstore.cpp \
    ../productcard.cpp \
    ../productcardstore.cpp

HEADERS += \
    tst_productcardstore.h \
    ../productcard.h \
    ../productcardstore.h

INCLUDEPATH += ..
