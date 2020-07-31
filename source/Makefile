CC=g++
LINKER_FLAGS= `pkg-config --cflags --libs gtk+-3.0`

../build/aplicacion:  main.cpp Aplicacion.o Widget.o Usuario.o Empresa.o Domicilio.o 
	$(CC) main.cpp $(LINKER_FLAGS) -o ../build/aplicacion Aplicacion.o Widget.o Usuario.o Empresa.o Domicilio.o

Aplicacion.o:	Aplicacion.cpp
	$(CC) $(LINKER_FLAGS) -c Aplicacion.cpp

Widget.o:	Widget.cpp
	$(CC) $(LINKER_FLAGS) -c Widget.cpp

Usuario.o:	Usuario.cpp
	$(CC) -c Usuario.cpp

Empresa.o:	Empresa.cpp
	$(CC) -c Empresa.cpp

Domicilio.o:	Domicilio.cpp
	$(CC) -c Domicilio.cpp
