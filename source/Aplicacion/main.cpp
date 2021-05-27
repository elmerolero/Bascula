#include "Aplicacion.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
using namespace std;



void actualizarTiempo( GtkWidget *widget, gpointer ptr );
void establecerUsuario( GtkWidget *widget, gpointer ptr );

int main( int argc, char *argv[] )
{
    // Activa la aplicacion
    aplicacionActiva = true;

    try{
        // Crea la aplicación 
        GtkApplication *aplicacion = gtk_application_new( "mx.com.minipluss.scb", G_APPLICATION_FLAGS_NONE );
        g_signal_connect( aplicacion, "activate", G_CALLBACK( iniciar ), nullptr );
        g_application_run( G_APPLICATION( aplicacion ), argc, argv );
        g_object_unref( aplicacion );
    }
    catch( exception &e ){
        // Crea un archivo de error
        ofstream log;

        // Se abre el archivo
        log.open( "../resources/log.txt", ios_base::out | ios_base::app );
        if( !log ){
            throw runtime_error( "Error" );
        }

        // Indica el tipo de error generado
        log << "[Fecha: " << obtenerFecha() << " Hora: " << obtenerHora() << "]: " << e.what() << endl;

        // Cierra el archivo
        log.close();
    }

    return 0; 
}
