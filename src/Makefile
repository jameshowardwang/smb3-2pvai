CC=g++
CFLAGS=-g -c -Wall -std=c++11
LDFLAGS=
SOURCES=main.cpp \
    GLInjectInput.cpp \
    Logger.cpp \
    SSRVideoStreamWatcher.cpp \
    SSRVideoStreamReader.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=smb3frameprocessor

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXECUTABLE)
