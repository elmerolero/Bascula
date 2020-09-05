CC=g++
LINKER_FLAGS= `pkg-config --cflags --libs gtk+-3.0 sqlite3`

../build/aplicacion:  main.cpp Aplicacion.o Widget.o Usuario.o Funciones.o Ticket.o Registro.o Database.o
	$(CC) main.cpp $(LINKER_FLAGS) -g -o ../build/aplicacion Aplicacion.o Widget.o Usuario.o Funciones.o Ticket.o Registro.o Database.o

Aplicacion.o:	Aplicacion.h Aplicacion.cpp
	$(CC) $(LINKER_FLAGS) -c Aplicacion.h Aplicacion.cpp

Widget.o:	Widget.h Widget.cpp
	$(CC) $(LINKER_FLAGS) -c Widget.cpp

Usuario.o:	Usuario.h Usuario.cpp
	$(CC) -c Usuario.cpp
	
Funciones.o: Funciones.h Funciones.cpp
	$(CC) $(LINKER_FLAGS) -c Funciones.h Funciones.cpp

Ticket.o: Ticket.h Ticket.cpp
	$(CC) -c Ticket.h Ticket.cpp

Registro.o: Registro.h Registro.cpp
	$(CC) -c Registro.h Registro.cpp

Database.o: Database.h Database.cpp
	$(CC) $(LINKER_FLAGS) `pkg-config --cflags sqlite3` -c Database.h Database.cpp `pkg-config --libs sqlite3`
