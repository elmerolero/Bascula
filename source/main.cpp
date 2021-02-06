#include "Aplicacion.h"
#include <iostream>
#include <thread>
#include <fstream>
#include <stdexcept>
#include "LectorBascula.h"
using namespace std;

void actualizarTiempo( GtkWidget *widget, gpointer ptr );
void establecerUsuario( GtkWidget *widget, gpointer ptr );

int main( int argc, char * argv[] )
{
    // Activa la aplicacion
    aplicacionActiva = true;
    thread lectorBascula( lectorBasculaActualizarPeso );

    try{
        // Inicializa gtk
        gtk_init( &argc, &argv );

        // Inicia la ventana principal
        iniciar();

        // Bucle principal
        gtk_main();
    }
    catch( exception &e ){
        // Crea un archivo de error
        ofstream log;

        // Se abre el archivo
        log.open( "../resources/log.txt", ios_base::out );
        if( !log ){
            throw runtime_error( "Error" );
        }

        // Indica el tipo de error generado
        log << e.what() << endl;

        // Cierra gtk
        gtk_main_quit();

        // Cierra el archivo
        log.close();
    }

    // Cierra el thread
    aplicacionActiva = false;
    CancelIoEx( dispositivo, NULL );
    lectorBascula.join();

    return 0; 
}
