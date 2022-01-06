CC=g++
CFLAGS= `pkg-config --cflags gtk+-3.0 sqlite3` -mwindows
LIBS= `pkg-config --libs gtk+-3.0 sqlite3`
OBJS= Database.o

main:  consultas.cpp $(OBJS)
	$(CC) consultas.cpp $(CFLAGS) -o main $(OBJS) $(LIBS)

Database.o: Database.h Database.cpp
	$(CC) $(CFLAGS) -c Database.h Database.cpp $(LIBS)

