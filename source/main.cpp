#include "Aplicacion.h"
#include <iostream>
using namespace std;

void actualizarTiempo( GtkWidget *widget, gpointer ptr );
void establecerUsuario( GtkWidget *widget, gpointer ptr );

int main( int argc, char * argv[] )
{
    // Inicializa gtk
    gtk_init( &argc, &argv );

    // Inicia la ventana principal
    iniciar();
	
    // Bucle principal
    gtk_main();

    return 0; 
}
