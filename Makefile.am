if DEBUG
AM_CFLAGS = -g3 -O0
AM_CXXFLAGS = -g3 -O0
else
AM_CFLAGS = -O2
AM_CXXFLAGS = -O2
endif

bin_PROGRAMS = sm
sm_SOURCES = main.c state-machine.c message-queue.c state-machine.h message-queue.h test-sm.c
AM_LDFLAGS = -pthread
