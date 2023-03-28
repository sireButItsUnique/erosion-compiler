default: erosion
	./erosion test.cor

erosion: main.cpp */*.cpp
	g++ main.cpp -o erosion

clean:
	rm erosion
