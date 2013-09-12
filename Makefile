CC   = gcc
CXX  = g++

ifeq ($(shell uname -s),Darwin)
DARWIN = -stdlib=libc++
else
DARWIN = 
endif


#DEBUG = -Wall -g -O0
RELEASE = -Wall -O1
CXXSTD = -std=c++11 $(DARWIN)
CSTD = -std=c99


DIRINCLUDES =

DIRINC = $(addprefix -I./, $(DIRINCLUDES) )

INC = -I./ -I./procstat -I./Calibrate $(DIRINC)

include $(addsuffix /Makefile, $(addprefix ./, $(DIRINCLUDES)) )

#STATIC 	= -static -static-libstdc++ -static-libgcc
CXXFLAGS = -Wall $(CXXSTD) $(RELEASE) $(DEBUG) $(INC)
CFLAGS 	= -Wall $(CSTD) $(RELEASE) $(DEBUG) $(INC)
EXE = svar


LIBS = $(STATIC) -lrt -lprocstat -lpthread -lm -lgsl -lgslcblas -lcalibrate
LDFLAGS = -L./procstat -L. -L./Calibrate

UNROLLED = noop_loop_unrolled
NOOP     = noop_loop

CPPOBJ = main command_arguments command_option_base process \
			load shm gate gatekeeper procwait \
         $(UNROLLED) $(NOOP)

COBJ	= system_query getrandom

FILES = $(addsuffix .cpp, $(CPPOBJ)) $(addsuffix .c, $(COBJ) )
OBJS  = $(addsuffix .o, $(CPPOBJ)) $(addsuffix .o, $(COBJ) )


CLEANLIST =  $(OBJS) $(EXE)\

.PHONY: all
all:  $(EXE)

$(EXE): $(FILES)
	$(MAKE) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIBS)



.PHONY: clean
clean:
	rm -rf $(CLEANLIST)
