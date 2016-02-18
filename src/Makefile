CC=g++
CFLAGS=-g -c -Wall -std=c++11 -pthread
LDFLAGS= -pthread
SOURCES=main.cpp \
    GLInjectInput.cpp \
    Logger.cpp \
    SSRVideoStreamWatcher.cpp \
    SSRVideoStreamReader.cpp \
    extractor/GameStateExtractor.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=smb3frameprocessor

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	mkdir -p ../build
	$(CC) $(LDFLAGS) $(OBJECTS) -o ../build/$@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o ../build/$(EXECUTABLE)
