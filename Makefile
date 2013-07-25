CC   = clang
CXX  = clang++

ifeq ($(shell uname -s),Darwin)
DARWIN = -stdlib=libc++
else
DARWIN = 
endif


DEBUG = -g
CFLAGS = -Wall -O0  $(DEBUG)
CXXFLAGS = -Wall -O0 -I$(ASTDIR) -I$(VISITORDIR) -I. $(DEBUG)
CSTD = -std=c99
CXXSTD = -std=c++11 $(DARWIN)


EXE = svar


CPPOBJ = main data command_arguments

LIBS = 
FILES = $(addsuffix .cpp, $(CPPOBJ))
OBJS  = $(addsuffix .o, $(CPPOBJ))


CLEANLIST =  $(OBJS)\

.PHONY: all
all:  $(EXE)

$(EXE): $(FILES)
	$(MAKE) $(OBJS)
	$(CXX) $(CXXFLAGS) $(CXXSTD) -o $(EXE) $(OBJS)


.PHONY: clean
clean:
	rm -rf $(CLEANLIST)
