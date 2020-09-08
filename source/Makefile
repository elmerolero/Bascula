CC=g++
CFLAGS= `pkg-config --cflags gtk+-3.0 sqlite3`
LIBS= `pkg-config --libs gtk+-3.0 sqlite3`

../build/aplicacion:  main.cpp Aplicacion.o Widget.o Usuario.o Funciones.o Ticket.o Registro.o Database.o
	$(CC) main.cpp $(CFLAGS) -g -o ../build/aplicacion Aplicacion.o Widget.o Usuario.o Funciones.o Ticket.o Registro.o Database.o $(LIBS)

Aplicacion.o:	Aplicacion.h Aplicacion.cpp
	$(CC) $(CFLAGS) -c Aplicacion.h Aplicacion.cpp $(LIBS)

Widget.o:	Widget.h Widget.cpp
	$(CC) $(CFLAGS) -c Widget.cpp $(LIBS)

Usuario.o:	Usuario.h Usuario.cpp
	$(CC) -c Usuario.cpp
	
Funciones.o: Funciones.h Funciones.cpp
	$(CC) $(CFLAGS) -c Funciones.h Funciones.cpp

Ticket.o: Ticket.h Ticket.cpp
	$(CC) -c Ticket.h Ticket.cpp

Registro.o: Registro.h Registro.cpp
	$(CC) -c Registro.h Registro.cpp

Database.o: Database.h Database.cpp
	$(CC) $(CFLAGS) -c Database.h Database.cpp $(LIBS)
