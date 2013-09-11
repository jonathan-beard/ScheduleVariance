CC   = clang
CXX  = clang++

ifeq ($(shell uname -s),Darwin)
DARWIN = -stdlib=libc++
else
DARWIN = 
endif


DEBUG = -Wall -g -O0
RELEASE = -O2
CXXSTD = -std=c++11 $(DARWIN)
CSTD = -std=c99


DIRINCLUDES = Calibrate

DIRINC = $(addprefix -I./, $(DIRINCLUDES) )

INC = -I./ -I./procstat $(DIRINC)

include $(addsuffix /Makefile, $(addprefix ./, $(DIRINCLUDES)) )

#STATIC = -static -static-libstdc++ -static-libgcc
CXXFLAGS = -no-integrated-as -Wall $(CXXSTD) $(DEBUG) $(INC)
CFLAGS = -no-integrated-as -Wall $(CSTD) $(DEBUG) $(INC)
EXE = svar


LIBS = $(STATIC) -lrt -lprocstat -lpthread -lm -lgsl -lgslcblas
LDFLAGS = -L./procstat -L.

UNROLLED = noop_loop_unrolled
NOOP     = noop_loop

CPPOBJ = main command_arguments command_option_base process \
			load shm gate gatekeeper procwait \
         $(UNROLLED) $(NOOP) $(CALIBRATECPPCODE)

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
