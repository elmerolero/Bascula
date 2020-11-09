CC=g++
CFLAGS= `pkg-config --cflags gtk+-3.0 sqlite3`
LIBS= `pkg-config --libs gtk+-3.0 sqlite3`
OBJS= Aplicacion.o Widget.o Usuario.o Funciones.o Ticket.o Registro.o Database.o ContenedorRegistros.o Vistas.o LectorBascula.o GestorRegistros.o Sesion.o sha256.o RegistrosInternos.o RegistrosPublicos.o TicketPublico.o

../build/aplicacion:  main.cpp $(OBJS)
	$(CC) main.cpp $(CFLAGS) -o ../build/aplicacion $(OBJS) $(LIBS)

GestorRegistros.o: GestorRegistros.h GestorRegistros.cpp
	$(CC) $(CFLAGS) -c GestorRegistros.h GestorRegistros.cpp

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
	
LectorBascula.o: LectorBascula.h LectorBascula.cpp
	$(CC) $(CFLAGS) -c LectorBascula.h LectorBascula.cpp $(LIBS)
	
Sesion.o: Sesion.h Sesion.cpp
	$(CC) $(CFLAGS) -c Sesion.h Sesion.cpp $(LIBS)
	
sha256.o: sha256.h sha256.cpp
	$(CC) -c sha256.h sha256.cpp
	
RegistrosInternos.o: RegistrosInternos.h RegistrosInternos.cpp
	$(CC) $(CFLAGS) -c RegistrosInternos.h RegistrosInternos.cpp $(LIBS)

RegistrosPublicos.o: RegistrosPublicos.h RegistrosPublicos.cpp
	$(CC) $(CFLAGS) -c RegistrosPublicos.h RegistrosPublicos.cpp $(LIBS)

TicketPublico.o: TicketPublico.h TicketPublico.cpp
	$(CC) -c TicketPublico.h TicketPublico.cpp
