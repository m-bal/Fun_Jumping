SFMLFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
EIGENFLAGS = -I/usr/include/eigen3/


all: main

main: main.cc
	g++ main.cc $(SFMLFLAGS) $(EIGENFLAGS) -o test

perlin.o: perlin.cc
	g++ -c perlin.cc

texture.o:
	g++ -c texture.cc

clean:
	rm test *.o




