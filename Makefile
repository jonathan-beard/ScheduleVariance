CC   = clang
CXX  = clang++

ifeq ($(shell uname -s),Darwin)
DARWIN = -stdlib=libc++
else
DARWIN = 
endif


DEBUG = -g -O0
CXXSTD = -std=c++11 $(DARWIN)
CSTD = -std=c99


INC = -I./procstat

OPS = 10000

CXXFLAGS = -Wall -O2 $(CXXSTD) $(DEBUG) $(INC)
CFLAGS = -Wall -O2 $(CSTD)  $(DEBUG) $(INC)


EXE = svar


LDFLAGS = -L./procstat -L.
LIBS = -lrt -lprocstat

CPPOBJ = main command_arguments command_option_base process \
			load noop_loop shm noop_loop_unrolled

COBJ	= system_query getrandom

FILES = $(addsuffix .cpp, $(CPPOBJ)) $(addsuffix .c, $(COBJ) )
OBJS  = $(addsuffix .o, $(CPPOBJ)) $(addsuffix .o, $(COBJ) )


CLEANLIST =  $(OBJS) $(EXE)\

.PHONY: all
all:  $(EXE)

$(EXE): $(FILES)
	./gen_noop_load.pl $(OPS)
	$(MAKE) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIBS)



.PHONY: clean
clean: 
	rm -rf $(CLEANLIST)
