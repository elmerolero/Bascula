#include <gtk/gtk.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "Aplicacion.h"
using namespace std;

void actualizarTiempo( GtkWidget *widget, gpointer ptr );
void establecerUsuario( GtkWidget *widget, gpointer ptr );

Aplicacion aplicacion;

int main( int argc, char * argv[] )
{
    // Inicializa gtk
    gtk_init( &argc, &argv );

    // Inicia la ventana principal
    aplicacion.iniciar();
	
	// Establece la hora
	actualizarTiempo( nullptr, nullptr );
	
	// Habilita actulizar la hora
	g_timeout_add( 1000, G_SOURCE_FUNC( actualizarTiempo ), aplicacion.obtenerWidget() );
	
    // Bucle principal
    gtk_main();

    return 0; 
}

void actualizarTiempo( GtkWidget *widget, gpointer ptr )
{
    stringstream stringTiempo;

    // Captura el momento
    auto momento = std::chrono::system_clock::now();
    
    // Convierte el momento obtenido en una estructura time_t
    time_t t  = std::chrono::system_clock::to_time_t( momento );

    // Obtiene en un estructura tm el tiempo local
    tm *tiempo = localtime( &t );

    // Establece la hora
    stringTiempo << "Hora: " << setfill( '0' ) << setw( 2 ) << ( ( tiempo -> tm_hour == 0 || tiempo -> tm_hour == 12 ) ? 12 : tiempo -> tm_hour % 12 ) 
				 << ":" << setw( 2 ) << to_string( tiempo -> tm_min ) << ":" << setw( 2 ) << to_string( tiempo -> tm_sec )
				 << ( ( tiempo -> tm_hour < 12 ) ? " am" : " pm" ) << setfill( ' ' );
	aplicacion.obtenerWidget()->establecerTextoEtiqueta( "Hora", stringTiempo.str() );
	
	// Limpia el buffer
	stringTiempo.str( "" );
	
	// Establece la fecha
    stringTiempo << "Fecha: " << setfill( '0' ) << setw( 2 ) << to_string( tiempo -> tm_mday ) << setfill( ' ' ) << " de " << meses.at( tiempo -> tm_mon ) << " de " << to_string( ( 1900 + tiempo -> tm_year ) );
	aplicacion.obtenerWidget()->establecerTextoEtiqueta( "Fecha", stringTiempo.str() );
}
