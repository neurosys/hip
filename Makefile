
CC=g++
PROJECT=test.exe
CFLAGS=-c -W -Wall -g
OUT=-o

all: main.o
	$(CC) main.o $(OUT) $(PROJECT)


main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp $(OUT) main.o


clean:
	rm -f *.o
	rm -f *.obj
	rm -f $(PROJECT)
