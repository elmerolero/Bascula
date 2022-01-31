#include "Widget.h"
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

GtkBuilder *builder = nullptr;
std::vector< GdkPixbuf * > imagenesRegistros;

void source_conectar( Source &fuente, GSourceFunc funcion ){
    fuente.source = g_idle_source_new();
    g_source_set_callback( fuente.source, funcion, NULL, NULL );
    fuente.source_id = g_source_attach( fuente.source, NULL );
}

void source_desconectar( Source &fuente ){
    if( fuente.source != NULL && fuente.source_id > 0 ){
        g_source_remove( fuente.source_id );
        g_source_destroy( fuente.source );
        fuente.source = nullptr;
        fuente.source_id = 0;
    }
}


GObject *buscar_objeto( string id ){
    return gtk_builder_get_object( builder, id.c_str() );
}

void conectar_senal( Signal &senal, GCallback callback, gpointer data ){
    cout << "conectar_senal" << endl;
    // Si hay una señal asociada al mismo, la desconecta
    if( senal.id > 0 ){
        desconectar_senal( senal );
    }

    // Obtiene el objeto indicado por el ID
    GObject *objeto = gtk_builder_get_object( builder, senal.object.c_str() );

    // Conecta la señal dada y la función dada con ese objeto
    senal.id = g_signal_connect( objeto, senal.type.c_str(), callback, data );
}

void desconectar_senal( Signal &senal ){
    cout << "desconectar_senal" << endl;
    // Obtiene el objeto indicado por la señal
    GObject *objeto = gtk_builder_get_object( builder, senal.object.c_str() );

    if( senal.id > 0 ){
        // Desconecta la señal
        g_signal_handler_disconnect( objeto, senal.id );
    }
    
    // Establece el identificador en cero
    senal.id = 0;
}

string obtener_nombre_elemento( string id ){
    GObject *objeto = gtk_builder_get_object( builder, id.c_str() );

    return gtk_widget_get_name( GTK_WIDGET( objeto ) );
}

void limpiar_contenedor( string idContenedor ){
    // Necesarios
    GList *hijos, *iterador;
   
    // Libera las imágenes de los registros contenedores
    for( GdkPixbuf *imagenRegistro : imagenesRegistros ){
        g_object_unref( imagenRegistro );
    }

    imagenesRegistros.clear();

    // Obtiene ol contenedor con el id indicado
    GObject *contenedor = buscar_objeto( idContenedor );
    hijos = gtk_container_get_children( GTK_CONTAINER( contenedor ) );
    for( iterador = hijos; iterador != NULL; iterador = g_list_next( iterador ) ){
        gtk_widget_destroy( GTK_WIDGET( iterador -> data ) );
    }
    
    // Libera las listas
    g_list_free( hijos );
}

const char *obtener_texto_entrada( string id ){
    // Obtiene el objeto que necesita
    GObject *objeto =  gtk_builder_get_object( builder, id.c_str() );
    
    // Retorna el objeto
    return gtk_entry_get_text( GTK_ENTRY( objeto ) );
}

void establecer_texto_entrada( string id, string texto ){
    // Obtiene la entrada con el id indicado
    GObject *objeto = gtk_builder_get_object( builder, id.c_str() );
    
    // Establece el texto en la entrada
    gtk_entry_set_text( GTK_ENTRY( objeto ), texto.c_str() );
}

void establecer_texto_etiqueta( string id, string texto ){
    // Obtiene el item solicitado
    GObject *item = gtk_builder_get_object( builder, id.c_str() );

    // Establece la etiqueta adecuada
    if( item == NULL ){
        throw runtime_error( "El objeto con el ID " + id + " no se encuentra." );
    }

    // Muestra el elemento
    gtk_label_set_text( GTK_LABEL( item ), texto.c_str() );
}

void mostrar_elemento( string id ){
    // Obtiene el elemento que se desea mostrar
    GObject const *objeto = gtk_builder_get_object( builder, id.c_str() );

    // ¿El objeto solicitado no se encuentra?
    if( objeto == NULL ){
        throw runtime_error( "El objeto con el ID " + id + " no se encuentra." );
    }
    
    // Se muestra el objeto
    gtk_widget_show( GTK_WIDGET( objeto ) );
}
    
void ocultar_elemento( string id ){
    // Obtiene el objeto indicado por la señal
    GObject *objeto = gtk_builder_get_object( builder, id.c_str() );

    // Se oculta el objeto
    gtk_widget_hide( GTK_WIDGET( objeto ) );
}
