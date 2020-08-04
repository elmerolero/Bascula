#include "Widget.h"
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

// Constructor sin argumentos
Widget::Widget():
    builder( nullptr ), object( nullptr ), error( nullptr )
{
    // Nada por hacer
}

// Constructor con la ubicación del archivo de interfaz
Widget::Widget( string directorio )
{
   // Carga la interfaz indicada por el directorio
   cargarInterfaz( directorio ); 
}

void Widget::cargarInterfaz( string directorio )
{
    // Inicializa los miembre de la clase
    builder = nullptr;
    object = nullptr;
    error = nullptr;

    // Crea el builder
    builder = gtk_builder_new();

    // Carga el archivo
    guint sucess = gtk_builder_add_from_file( builder, directorio.c_str(), &error );
    if( sucess == 0 ){
        gtk_widget_destroy( GTK_WIDGET(  builder ) );
        throw runtime_error( "Error al cargar el archivo: " + directorio );
    }
}

void Widget::conectarSenal( string id, string tipoSenal, GCallback funcion, gpointer data )
{
    // Obtiene el objeto indicado por el ID
    object = obtenerObjeto( id );

    // Conecta la señal dada y la función dada con ese objeto
    g_signal_connect( object, tipoSenal.c_str(), funcion, data);
}

GObject *Widget::obtenerObjeto( string id ) const
{
    return gtk_builder_get_object( builder, id.c_str() );
}

void Widget::insertarElementoAGrid( const Widget *elemento, string idFuente, string idDestino, gint left, gint top, gint width, gint height )
{
    // ¿El Widget no está vacío?
    if( elemento != nullptr ){
        // Obtiene el objeto con idFuente y el objeto con idDestino
        GObject *objetoFuente = elemento -> obtenerObjeto( idFuente );
        GObject *objetoDestino = obtenerObjeto( idDestino );

        // ¿Ambos elementos a insertar existen?
        if( objetoFuente != NULL && objetoDestino != NULL ){
            // ¿No ha sido insertado en el contenedor que se desea insertar?
            if( GTK_IS_CONTAINER( objetoDestino ) ){
                GList *children = gtk_container_get_children( GTK_CONTAINER( objetoDestino ) );
                if( g_list_find( children, objetoFuente ) == NULL ){
                    // Se agrega
                    gtk_grid_attach( GTK_GRID( objetoDestino ), GTK_WIDGET( objetoFuente ), left, top, width, height );
                }
            }
        }
    }
}

void Widget::establecerTextoEtiqueta( std::string idLabel, std::string texto )
{
    // Obtiene el item solicitado
    GObject *item = gtk_builder_get_object( builder, idLabel.c_str() );

    // Establece la etiqueta adecuada
    if( item != NULL ){
        gtk_label_set_text( GTK_LABEL( item ), texto.c_str() );
    }
}

GtkBuilder *Widget::obtenerBuilder() const
{
    return builder;
}


void Widget::destruir()
{
    gtk_widget_destroy( GTK_WIDGET( builder ) );
}

