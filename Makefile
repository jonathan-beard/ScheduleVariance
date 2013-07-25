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


CPPOBJ = main command_arguments command_option_base

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
