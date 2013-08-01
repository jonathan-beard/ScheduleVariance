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
ifeq ($(shell uname -s | grep -o Linux),Linux)
   CLOCKFREQ=get_clockfreq
   L = -lrt
   ifeq ($(shell uname -v | grep -o Ubuntu),Ubuntu)
      LIBRT = /usr/lib/$(shell uname -m)-linux-gnu/librt.a
   else
      ifeq ($(shell uname -m | grep -o x86_64),x86_64)
         LIBRT = /usr/lib64/librt.a
      else 
         LIBRT = /usr/lib/librt.a
      endif
   endif
else
   CLOCKFREQ=
   L = 
endif


CPPOBJ = main command_arguments command_option_base heavy_process process \
			load noop_loop

COBJ	= system_query			

LIBS = -lrt
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
