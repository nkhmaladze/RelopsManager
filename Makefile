# makefile for swatdb relops select and project assignment
#
# make: builds unittests
# make clean: cleans up compiled files
#


# location of make.config
SWATCONFIGDIR = ./
# defines SWATDBDIR
include $(SWATCONFIGDIR)/make.config

# path to SwatDB library files
LIBDIR = $(SWATDBDIR)lib/

# paths to include directories
INCLUDES = -I. -I$(SWATDBDIR)include/

# compiler
CC = g++

# compiler flags for test code build
CFLAGS =  -g -Wall #-pthread

# lflags for linking
LFLAGS = -L$(LIBDIR)

# swatdb and other libraries to link in 
LIBS = $(LFLAGS) -lswatdb -lm -pthread -lcrypto -lssl -lUnitTest++


SRCS = filescan.cpp relopsmgr_selects.cpp indexscan.cpp  relopsmgr.cpp \
       select.cpp project.cpp relopsmgr_projects.cpp operation.cpp  

# suffix replacement rule
OBJS = $(SRCS:.cpp=.o)

# be very careful to not add any spaces to ends of these
TARGET1 = projecttests
TARGET2 = selecttests
TARGET3 = smalltests

# generic makefile
.PHONY: clean 

all: $(TARGET1) $(TARGET2)  $(TARGET3) mkfiles

$(TARGET1): $(OBJS) $(TARGET1).cpp *.h *.sh
	./mktestconf.sh
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET1) $(TARGET1).cpp $(OBJS) $(LIBS)

$(TARGET2): $(OBJS) $(TARGET2).cpp *.h *.sh
	./mktestconf.sh
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET2) $(TARGET2).cpp $(OBJS) $(LIBS)

$(TARGET3): $(OBJS) $(TARGET3).cpp *.h *.sh
	./mktestconf.sh
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET3) $(TARGET3).cpp $(OBJS) $(LIBS)

mkfiles:
	./getfiles.sh

# suffix replacement rule using autmatic variables:
# automatic variables: $< is the name of the prerequiste of the rule
# (.cpp file),  and $@ is name of target of the rule (.o file)
.cpp.o: $(SRCS) *.h *.sh
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

runtests: 
	./$(TARGET1)
	sleep 2
	./$(TARGET2)
	sleep 2
	./$(TARGET3)

clean:
	./cleanup.sh
	$(RM) *.o $(TARGET1) $(TARGET2)  $(TARGET3) *.rel
