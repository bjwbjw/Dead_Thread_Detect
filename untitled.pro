TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

HEADERS += \
    mutex_lock.h

QMAKE_CXXFLAGS += -std=c++0x

LIBS +=-lpthread
