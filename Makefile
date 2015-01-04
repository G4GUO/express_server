CC            = gcc
CXX           = g++

OBJECTS = $(patsubst %.cpp,%.o, $(wildcard *.cpp))

express_server: $(OBJECTS)
	gcc -o $@ $^ -lpthread -lusb-1.0 -lm -lstdc++

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -f *.o	

install:
	mv express_server /usr/bin/express_server
	cp NarrowbandDVBS.rbf /lib/firmware/datvexpress/NarrowbandDVBS.rbf

