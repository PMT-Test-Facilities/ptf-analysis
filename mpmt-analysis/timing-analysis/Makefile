#**********************************************************************
# Author: John Walker
# Email:  jmgwalker@triumf.ca
# Date:   2019/09/25
#********************************************************************** 
INCDIR= $(shell pwd)/../../include
SRCDIR= $(shell pwd)/../../src
OBJDIR= $(shell pwd)/../../obj
BINDIR= $(shell pwd)/../../bin

VPATH = $(SRCDIR)

CFLAGS=-c -g -Wall `root-config --cflags` -I${INCDIR}
LDFLAGS=`root-config --glibs` -lHistPainter -lMinuit -L${ROOTSYS}/lib 

all: mpmt_timing_analysis.exe


mpmt_timing_analysis.exe:  mpmt_timing_analysis.o WaveformFitResult.o
	CPATH=/usr/local/include $(CXX) $^ -o $@ $(LDFLAGS)

mpmt_timing_analysis.o: mpmt_timing_analysis.cpp
	$(CXX) $(CFLAGS) $< -o $@

WaveformFitResult.o: ${SRCDIR}/WaveformFitResult.cpp
	$(CXX) $(CFLAGS) $< -o $@



clean:
	- $(RM) *.exe *.o
