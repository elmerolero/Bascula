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

gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
  guint width, height;
  GdkRGBA color;
  GtkStyleContext *context;

  context = gtk_widget_get_style_context (widget);

  width = gtk_widget_get_allocated_width (widget);
  height = gtk_widget_get_allocated_height (widget);

  gtk_render_background (context, cr, 0, 0, width, height);

  cairo_arc (cr,
             width / 2.0, height / 2.0,
             MIN (width, height) / 2.0,
             0, 2 * G_PI);

  gtk_style_context_get_color (context,
                               gtk_style_context_get_state (context),
                               &color);
  gdk_cairo_set_source_rgba (cr, &color);

  cairo_fill (cr);

 return FALSE;
}