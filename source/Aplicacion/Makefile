CC=g++
CFLAGS= `pkg-config --cflags gtk+-3.0 sqlite3`
LIBS= `pkg-config --libs gtk+-3.0 sqlite3`
OBJS= Aplicacion.o Widget.o Usuario.o Funciones.o Empresa.o Registro.o Database.o Vistas.o GestorRegistros.o Sesion.o sha256.o PesajesInternos.o PesajesPublicos.o Senales.o Imagen.o Producto.o Domicilio.o Basculas.o aplicacion.res

../../build/aplicacion:  main.cpp $(OBJS)
	$(CC) main.cpp $(CFLAGS) -lwinspool -mwindows -g -o ../../build/aplicacion $(OBJS) $(LIBS)

GestorRegistros.o: GestorRegistros.h GestorRegistros.cpp
	$(CC) $(CFLAGS) -c -g GestorRegistros.h GestorRegistros.cpp

Aplicacion.o:	Aplicacion.h Aplicacion.cpp
	$(CC) $(CFLAGS) -c -g Aplicacion.h Aplicacion.cpp $(LIBS)

Widget.o:	Widget.h Widget.cpp
	$(CC) $(CFLAGS) -c -g Widget.h Widget.cpp $(LIBS)

Usuario.o:	Usuario.h Usuario.cpp
	$(CC) $(CFLAGS) -c -g Usuario.h Usuario.cpp $(LIBS)
	
Funciones.o: Funciones.h Funciones.cpp
	$(CC) $(CFLAGS) -c -g Funciones.h Funciones.cpp

Registro.o: Registro.h Registro.cpp
	$(CC) -c -g Registro.h Registro.cpp

Database.o: Database.h Database.cpp
	$(CC) $(CFLAGS) -g -c Database.h Database.cpp $(LIBS)

Vistas.o: Vistas.h Vistas.cpp
	$(CC) $(CFLAGS) -c -g Vistas.h Vistas.cpp $(LIBS)
	
Sesion.o: Sesion.h Sesion.cpp
	$(CC) $(CFLAGS) -c -g Sesion.h Sesion.cpp $(LIBS)
	
sha256.o: sha256.h sha256.cpp
	$(CC) -c -g sha256.h sha256.cpp
	
PesajesInternos.o: PesajesInternos.h PesajesInternos.cpp
	$(CC) $(CFLAGS) -c -g PesajesInternos.h PesajesInternos.cpp $(LIBS)

PesajesPublicos.o: PesajesPublicos.h PesajesPublicos.cpp
	$(CC) $(CFLAGS) -c -g PesajesPublicos.h PesajesPublicos.cpp $(LIBS)

Senales.o: Senales.h Senales.cpp
	$(CC) $(CFLAGS) -c Senales.h Senales.cpp $(LIBS)

Basculas.o: Basculas.h Basculas.cpp
	$(CC) $(CFLAGS) -c Basculas.h Basculas.cpp $(LIBS)

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
