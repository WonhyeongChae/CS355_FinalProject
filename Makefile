PRG=out.exe 

GCC=g++
GCCFLAGS=-O2 -Wall -Wextra -std=c++20 -pedantic -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder

# I want to add all header files in src/include/
OBJECTS0= src/include/*.h src/skip_list.cpp src/linked_list.cpp
DRIVER0=src/main.cpp

VALGRIND_OPTIONS=-q --leak-check=full

DIFF_OPTIONS=--strip-trailing-cr --ignore-trailing-space

OSTYPE := $(shell uname)
ifeq (,$(findstring CYGWIN,$(OSTYPE)))
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif


gcc0:
	$(GCC) -o $(PRG) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS) 
0 1 2 3 4 5 6 7:
	@echo "running test$@"
	./build/$(PRG) > test$@.txt
clean:
	rm -f *.exe *.o