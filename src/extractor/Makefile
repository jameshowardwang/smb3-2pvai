CC=g++
CFLAGS=-g -c -Wall -std=c++11
LDFLAGS=
SOURCES=GameStateExtractorTestMain.cpp \
    GameStateExtractor.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=gsextract

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	mkdir -p ../../build
	$(CC) $(LDFLAGS) $(OBJECTS) -o ../../build/$@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o ../../build/$(EXECUTABLE)
