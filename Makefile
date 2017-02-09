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
	mkdir -p "/lib/firmware/datvexpress"
	cp express_server /usr/bin/express_server
	cp datvexpressdvbs.rbf /lib/firmware/datvexpress/datvexpressdvbs.rbf
	cp datvexpress8.ihx /lib/firmware/datvexpress/datvexpress8.ihx
	cp 10-datvexpress.rules /etc/udev/rules.d/10-datvexpress.rules
	@echo "======================================"
	@echo "|| You will now have to do a reboot ||"
	@echo "======================================"

