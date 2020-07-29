#include <gtk/gtk.h>

int main( int argc, char **argv )
{
    GtkBuilder *builder{ nullptr };
    GObject *object{ nullptr };
    GError *error{ nullptr };

    gtk_init( &argc, &argv );

    /* Construye una instancia GtkBuilder y carga la descripción de interfaz de usuario */
    builder = gtk_builder_new();
    if( gtk_builder_add_from_file( builder, "../resources/interfaces/Pesaje.glade", &error ) == 0 ){
        g_printerr( "Error loading file: %s\n", error -> message );
        g_clear_error( &error );
        return 1;
    }

    object = gtk_builder_get_object( builder, "VentanaEntradaSalida" );
    g_signal_connect( object, "destroy", G_CALLBACK( gtk_main_quit ), nullptr );

    gtk_main();

    return 0;
}

