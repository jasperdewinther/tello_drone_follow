LIBS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -lpthread `pkg-config opencv4 --cflags --libs` -lzbar
INCLUDE=-I/usr/include/opencv4/

CXX := g++ -O3 -Wall 

objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

%.o: %.cpp
	$(CXX) $(INCLUDE) -c $< -o $@

game: $(objects)
	$(CXX) $(INCLUDE) -o $@ $(objects) $(LIBS)

clean:
	rm -f game *.o

install:
	@echo '** Installing...'
	cp thegame /usr/bin/

uninstall:
	@echo '** Uninstalling...'
	$(RM) /usr/bin/thegame