#include "Aplicacion.h"
#include <iostream>
#include <thread>
#include "LectorBascula.h"
using namespace std;

void actualizarTiempo( GtkWidget *widget, gpointer ptr );
void establecerUsuario( GtkWidget *widget, gpointer ptr );

int main( int argc, char * argv[] )
{
    // Inicializa gtk
    gtk_init( &argc, &argv );

    // Inicia la ventana principal
    iniciar();

	// Activa la aplicacion
    aplicacionActiva = true;
    thread lectorBascula( lectorBasculaActualizarPeso );
	
    // Bucle principal
    gtk_main();

    // Cierra el thread
    aplicacionActiva = false;
    CancelIoEx( dispositivo, NULL );
    lectorBascula.join();

    return 0; 
}
