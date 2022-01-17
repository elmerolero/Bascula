CC=g++
CFLAGS= `pkg-config --cflags gtk+-3.0 sqlite3` -mwindows
LIBS= `pkg-config --libs gtk+-3.0 sqlite3`
OBJS= Aplicacion.o Widget.o Usuario.o Funciones.o Ticket.o Empresa.o Registro.o Database.o ContenedorRegistros.o Vistas.o GestorRegistros.o Sesion.o sha256.o RegistrosInternos.o RegistrosPublicos.o TicketPublico.o Senales.o Bascula.o GestorBasculas.o Imagen.o Producto.o Domicilio.o aplicacion.res

../../build/aplicacion:  main.cpp $(OBJS)
	$(CC) main.cpp $(CFLAGS) -g -o ../../build/aplicacion $(OBJS) $(LIBS)

GestorRegistros.o: GestorRegistros.h GestorRegistros.cpp
	$(CC) $(CFLAGS) -c GestorRegistros.h GestorRegistros.cpp

Aplicacion.o:	Aplicacion.h Aplicacion.cpp
	$(CC) $(CFLAGS) -c Aplicacion.h Aplicacion.cpp $(LIBS)

Widget.o:	Widget.h Widget.cpp
	$(CC) $(CFLAGS) -c Widget.h Widget.cpp $(LIBS)

Usuario.o:	Usuario.h Usuario.cpp
	$(CC) $(CFLAGS) -c Usuario.h Usuario.cpp $(LIBS)
	
Funciones.o: Funciones.h Funciones.cpp
	$(CC) $(CFLAGS) -c Funciones.h Funciones.cpp

Ticket.o: Ticket.h Ticket.cpp
	$(CC) -c Ticket.h Ticket.cpp

Registro.o: Registro.h Registro.cpp
	$(CC) -c Registro.h Registro.cpp

Database.o: Database.h Database.cpp
	$(CC) $(CFLAGS) -g -c Database.h Database.cpp $(LIBS)

ContenedorRegistros.o: ContenedorRegistros.h ContenedorRegistros.cpp
	$(CC) $(CFLAGS) -c ContenedorRegistros.h ContenedorRegistros.cpp $(LIBS)

Vistas.o: Vistas.h Vistas.cpp
	$(CC) $(CFLAGS) -c Vistas.h Vistas.cpp $(LIBS)
	
Sesion.o: Sesion.h Sesion.cpp
	$(CC) $(CFLAGS) -c Sesion.h Sesion.cpp $(LIBS)
	
sha256.o: sha256.h sha256.cpp
	$(CC) -c sha256.h sha256.cpp
	
RegistrosInternos.o: RegistrosInternos.h RegistrosInternos.cpp
	$(CC) $(CFLAGS) -c RegistrosInternos.h RegistrosInternos.cpp $(LIBS)

RegistrosPublicos.o: RegistrosPublicos.h RegistrosPublicos.cpp
	$(CC) $(CFLAGS) -c RegistrosPublicos.h RegistrosPublicos.cpp $(LIBS)

TicketPublico.o: TicketPublico.h TicketPublico.cpp
	$(CC) -c TicketPublico.h TicketPublico.cpp $(LIBS)

Senales.o: Senales.h Senales.cpp
	$(CC) $(CFLAGS) -c Senales.h Senales.cpp $(LIBS)

GestorBasculas.o: GestorBasculas.h GestorBasculas.cpp
	$(CC) $(CFLAGS) -g -c GestorBasculas.h GestorBasculas.cpp $(LIBS)

Bascula.o: Bascula.h Bascula.cpp
	$(CC) $(CFLAGS) -c Bascula.h Bascula.cpp $(LIBS)

Imagen.o: Imagen.h Imagen.cpp
	$(CC) $(CFLAGS) -c Imagen.h Imagen.cpp $(LIBS)

Producto.o: Producto.h Producto.cpp
	$(CC) $(CFLAGS) -c Producto.h Producto.cpp $(LIBS)

Empresa.o: Empresa.h Empresa.cpp
	$(CC) $(CFLAGS) -c Empresa.h Empresa.cpp $(LIBS)

Domicilio.o: Domicilio.h Domicilio.cpp
	$(CC) $(CFLAGS) -c Domicilio.h Domicilio.cpp $(LIBS)

aplicacion.res: aplicacion.rc
	windres aplicacion.rc -O coff -o aplicacion.res
