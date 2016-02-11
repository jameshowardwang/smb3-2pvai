CC=g++-4.6
CFLAGS=-g -c -Wall
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
