CC=g++
CFLAGS= `pkg-config --cflags gtk+-3.0 sqlite3`
LIBS= `pkg-config --libs gtk+-3.0 sqlite3`
OBJS= Aplicacion.o Widget.o Usuario.o Funciones.o Ticket.o Registro.o Database.o ContenedorRegistros.o Vistas.o

../build/aplicacion:  main.cpp Aplicacion.o Widget.o Usuario.o Funciones.o Ticket.o Registro.o Database.o ContenedorRegistros.o Vistas.o
	$(CC) main.cpp $(CFLAGS) -g -o ../build/aplicacion $(OBJS) $(LIBS)

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

ContenedorRegistros.o: ContenedorRegistros.h ContenedorRegistros.cpp
	$(CC) $(CFLAGS) -c ContenedorRegistros.h ContenedorRegistros.cpp $(LIBS)

Vistas.o: Vistas.h Vistas.cpp
	$(CC) $(CFLAGS) -c Vistas.h Vistas.cpp $(LIBS)
