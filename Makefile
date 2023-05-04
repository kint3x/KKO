#Makefile for compressor
# Martin Matejka xmatej55 KKO 2022/23

CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=huff_codec

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

comp: all
	rm -f *.compressed
	./huff_codec -i data/df1hvx.raw -w 512 -c

decomp: all
	./huff_codec -i out.compressed -d

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) 