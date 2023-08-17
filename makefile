CC = g++ 
CFLAGS = -Wall -g

LIBS = -lSDL2 -lSDL2_ttf
OBJECTS = fileio.o renderer.o ImageFilters.o starDetectionAlgorithm.o Stars.o

main:  $(OBJECTS) main.cpp
	$(CC) $(CFLAGS) $(OBJECTS) main.cpp $(LIBS) -o main

renderer.o: renderer.h  renderer.cpp
	$(CC) $(CFLAGS) $(LIBS) -c renderer.cpp

fileio.o: fileio.h fileio.cpp
	$(CC) $(CFLAGS) $(LIBS) -c fileio.cpp

ImageFilters.o:	ImageFilters.h ImageFilters.cpp
	$(CC) $(CFLAGS) $(LIBS) -c ImageFilters.cpp

starDetectionAlgorithm.o: starDetectionAlgorithm.h starDetectionAlgorithm.cpp
	$(CC) $(CFLAGS) $(LIBS) -c starDetectionAlgorithm.cpp

Stars.o: Stars.h Stars.cpp
	$(CC) $(CFLAGS) $(LIBS) -c Stars.cpp

clean:
	rm *.o*
	rm *~
