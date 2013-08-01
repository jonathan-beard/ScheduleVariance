CC   = clang
CXX  = clang++

ifeq ($(shell uname -s),Darwin)
DARWIN = -stdlib=libc++
else
DARWIN = 
endif


DEBUG = -g
CXXSTD = -std=c++11 $(DARWIN)
CSTD = -std=c99

CXXFLAGS = -Wall -O0 $(CXXSTD) $(DEBUG)
CFLAGS = -Wall -O0 $(CSTD)  $(DEBUG)

EXE = svar

CPPOBJ = main command_arguments command_option_base heavy_process process \
			load noop_loop

COBJ	= system_query			

FILES = $(addsuffix .cpp, $(CPPOBJ)) $(addsuffix .c, $(COBJ) )
OBJS  = $(addsuffix .o, $(CPPOBJ)) $(addsuffix .o, $(COBJ) )


CLEANLIST =  $(OBJS) $(EXE)\

.PHONY: all
all:  $(EXE)

$(EXE): $(FILES)
	$(MAKE) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS) $(LIBS)

.PHONY: clean
clean:
	rm -rf $(CLEANLIST)
