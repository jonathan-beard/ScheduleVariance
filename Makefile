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


INC = -I./procstat

OPS = 100

CXXFLAGS = -Wall $(CXXSTD) $(DEBUG) $(INC)
CFLAGS = -Wall  $(CSTD)  $(DEBUG) $(INC)


EXE = svar


LDFLAGS = -L./procstat -L.
LIBS = -lrt -lprocstat

CPPOBJ = main command_arguments command_option_base process \
			load shm \
         noop_loop
         #noop_loop_clock
         #noop_loop_unrolled 

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
