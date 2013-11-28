# chsrv build script

Sources = $(wildcard *.cpp)
Objects = $(patsubst %.cpp, %.o, $(Sources))

all:
	make bin/chsrv

run:
	make all
	bin/chsrv test.map

bin/chsrv: $(Objects)
	g++ -o bin/chsrv -pthread $(Objects)

%.o: %.cpp Makefile
	g++ -c -o $*.o $*.cpp
