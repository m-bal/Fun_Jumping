SFMLFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
EIGENFLAGS = -I/usr/include/eigen3/

all: main

main: main.cc
	g++ main.cc $(SFMLFLAGS) $(EIGENFLAGS) -o test

clean:
	rm -rf test 




