CC=g++
CFLAGS= -mwindows -Iinclude/SDL2 -Iinclude/lzip `pkg-config --cflags gtk+-3.0 sqlite3` 
LIBS= -lzip `pkg-config --libs gtk+-3.0 sqlite3` 

../../build/actualizador:  main.cpp 
	$(CC) main.cpp $(CFLAGS) -o ../../build/actualizador $(LIBS)
