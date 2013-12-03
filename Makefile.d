# chsrv build script

Sources = $(wildcard *.cpp)
Objects = $(patsubst %.cpp, %.o, $(Sources))

all:
	make -fMakefile.d bin/chsrv

run:
	make -fMakefile.d all
	bin/chsrv bin/sample02_k.map

bin/chsrv: $(Objects)
	g++ -g -Wall -O2 -o bin/chsrv -pthread $(Objects)

%.o: %.cpp Makefile
	g++ -g -Wall -O2 -c -o $*.o $*.cpp
