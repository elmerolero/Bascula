#include "Widget.h"
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

// Constructor sin argumentos
Widget::Widget():
    builder( nullptr )
{
    // Nada por hacer
}

// Destructor
Widget::~Widget()
{
    
}

// Constructor con la ubicación del archivo de interfaz
Widget::Widget( string directorio )
{
   // Carga la interfaz indicada por el directorio
   cargarWidget( directorio ); 
}

// Carga un Widget guardado en un archivo XML
void Widget::cargarWidget( string archivo )
{
    // Elemento para obtener los errores que ocurran al cargar el widget
    GError *error = nullptr;

    // Crea el builder
    builder = gtk_builder_new();

    // ¿Ocurre algún error al cargar el archivo?
    if( gtk_builder_add_from_file( builder, archivo.c_str(), &error ) == 0 ){
        // Lanza una excepcion
        throw runtime_error( "Error al cargar el archivo " + archivo + "." );
    }
}

// Establece el icono de la ventana
void Widget::establecerIconoVentana( string idVentana, string archivo )
{
    GObject *objeto = obtenerObjeto( idVentana );

    gtk_window_set_icon_from_file( GTK_WINDOW( objeto ), archivo.c_str(), NULL );
}

// Obtiene el objeto del Widget de acuero al id indicado
GObject *Widget::obtenerObjeto( string id ) const
{
    return gtk_builder_get_object( builder, id.c_str() );
}

// Conecta la señal a una función
guint Widget::conectarSenal( string id, string tipoSenal, GCallback funcion, gpointer data )
{
    // Obtiene el objeto indicado por el ID
    GObject *object = obtenerObjeto( id );

    // Conecta la señal dada y la función dada con ese objeto
    return g_signal_connect( object, tipoSenal.c_str(), funcion, data);
}

// Desconecta la señal dado el id correspondiente
void Widget::desconectarSenal( string idElemento, guint idSenal )
{
    // Obtiene el objeto indicado por la señal
    GObject *object = obtenerObjeto( idElemento );

    // Desconecta la señal
    g_signal_handler_disconnect( object, idSenal );
}

// Inserta el elemento de un Widget dado dentro de otro del uno de los elementos de este Widget
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
                // Obtiene el grid al que se desea insertar
                GList *children = gtk_container_get_children( GTK_CONTAINER( objetoDestino ) );

                // ¿El elemento ya ha sido añadido anteriormente?
                if( g_list_find( children, objetoFuente ) == NULL ){
                    // Se agrega
                    gtk_grid_attach( GTK_GRID( objetoDestino ), GTK_WIDGET( objetoFuente ), left, top, width, height );
                }
            }
        }
    }
}

// Elimina el Widget dado
void Widget::eliminarElemento( const Widget *elemento, string idElemento, string idContenedor )
{
    // Contenedor
    GObject *contenedor = obtenerObjeto( idContenedor );
    
    // Widget a retirar
    GObject *objeto = elemento -> obtenerObjeto( idElemento );
    
    //
    gtk_container_remove( GTK_CONTAINER( contenedor ), GTK_WIDGET( objeto ) );
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

void Widget::establecerBotonEtiqueta( string idBoton, string texto )
{
    // Obtiene el item solicitado
    GObject *item = gtk_builder_get_object( builder, idBoton.c_str() );

    // Establece la etiqueta adecuada
    if( item != NULL ){
        gtk_button_set_label( GTK_BUTTON( item ), texto.c_str() );
    }
}

void Widget::mostrarElemento( string id ) const
{
    // Obtiene el elemento que se desea mostrar
    GObject const *objeto = obtenerObjeto( id );

    // ¿El objeto solicitado no se encuentra?
    if( objeto == NULL ){
        throw runtime_error( "El objeto con el ID " + id + " no se encuentra." );
    }
    
    // Se muestra el objeto
    gtk_widget_show( GTK_WIDGET( objeto ) );
}

void Widget::ocultarElemento( string id ) const
{
    // Obtiene el elemento que se desea mostrar
    GObject const *objeto = obtenerObjeto( id );

    // ¿El objeto solicitado no se encuentra?
    if( objeto == NULL ){
        throw runtime_error( "El objeto con el ID " + id + "no se encuentra." );
    }
    
    // Se muestra el objeto
    gtk_widget_hide( GTK_WIDGET( objeto ) );
}

const GtkBuilder *Widget::obtenerBuilder() const
{
    return builder;
}

const char *Widget::obtenerTextoEtiqueta( std::string idEntrada )
{
    // Obtiene la etiqueta solicitada
    GObject *objeto = obtenerObjeto( idEntrada );
    
    return gtk_label_get_text( GTK_LABEL( objeto ) );
}

void Widget::establecerTamanoEntrada( std::string idEntrada, int tamano )
{
    // Verifica que esté dentro del tamaño adecuado
    if( tamano < -1 ){
        throw invalid_argument( "Tamaño a establecer fuera de rango." );
    }
    
    // Obtiene la entrada con el id indicado
    GObject *objeto = obtenerObjeto( idEntrada );
    
    gtk_entry_set_max_length( GTK_ENTRY( objeto ), tamano );
}

void Widget::establecerCompletadorEntrada( std::string idEntrada, GtkEntryCompletion *completador )
{
    GObject *objecto = obtenerObjeto( idEntrada );
    gtk_entry_set_completion( GTK_ENTRY( objecto ), completador );
}

void Widget::establecerTextoEntrada( std::string idEntrada, std::string texto )
{
    // Obtiene la entrada con el id indicado
    GObject *objeto = obtenerObjeto( idEntrada );
    
    gtk_entry_set_text( GTK_ENTRY( objeto ), texto.c_str() );
}

const char *Widget::obtenerTextoEntrada( std::string idEntrada )
{
    // Obtiene el objeto que necesita
    GObject *objeto = obtenerObjeto( idEntrada );
    
    return gtk_entry_get_text( GTK_ENTRY( objeto ) );
}

void Widget::removerElementosHijos( std::string idContenedor )
{
    // Necesarios
    GList *hijos, *iterador;
   
    // Obtiene ol contenedor con el id indicado
    GObject *contenedor = obtenerObjeto( idContenedor );
    hijos = gtk_container_get_children( GTK_CONTAINER( contenedor ) );
    for( iterador = hijos; iterador != NULL; iterador = g_list_next( iterador ) ){
        gtk_widget_destroy( GTK_WIDGET( iterador -> data ) );
    }
    
    // Libera las listas
    g_list_free( hijos );
}

void Widget::destruir()
{
    gtk_widget_destroy( GTK_WIDGET( builder ) );
}
