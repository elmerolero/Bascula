CC=g++
LINKER_FLAGS= `pkg-config --cflags --libs gtk+-3.0`

../build/aplicacion: aplicacion.cpp
	$(CC) aplicacion.cpp $(LINKER_FLAGS) -o ../build/aplicacion

