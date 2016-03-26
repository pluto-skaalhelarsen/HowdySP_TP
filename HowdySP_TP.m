# makefile for HowdySP_TP exploration of the Numenta NuPIC code
CC=g++
LINK=g++
CFLAGS= -c -std=gnu++11 -I $(NUPIC_CORE)/src
LFLAGS= -std=gnu++11
LIBS= $(NUPIC_CORE)/lib/_algorithms.so  $(NUPIC_CORE)/lib/_engine_internal.so

OBJS=HowdySP_TP.o CmdLineParse.o BufferSummary.o
ICOMMON=

all: HowdySP_TP

HowdySP_TP.o : HowdySP_TP.cpp
	$(CC) $(CFLAGS) $*.cpp

CmdLineParse.o : CmdLineParse.cpp
	$(CC) $(CFLAGS) $*.cpp


HowdySP_TP:	$(OBJS)
	$(LINK) $(LFLAGS) -o HowdySP_TP $(OBJS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $*.cpp

clean:
	rm *.o
	rm HowdySP_TP

