
all: main

main: main.cc perlin.o texture.o
	g++ main.cc perlin.o -lm -lGL -lGLU -lX11 -o test

perlin.o: perlin.cc
	g++ -c perlin.cc

texture.o:
	g++ -c texture.cc

clean:
	rm test *.o




