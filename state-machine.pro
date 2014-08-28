TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    state-machine.c \
    message-queue.c \
    test-sm.c

HEADERS += \
    state-machine.h \
    message-queue.h \
    test-sm.h

LIBS += -pthread
