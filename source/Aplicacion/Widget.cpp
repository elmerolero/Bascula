#include "Widget.h"
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

GtkBuilder *builder = nullptr;
std::vector< GdkPixbuf * > imagenesRegistros;

// Constructor sin argumentos
Widget::Widget():
    builder( nullptr )
{
    // Nada por hacer
}

/*void leer_widget_seleccionado_listbox( string id ){
    // Busca el renglón seleccionado y si no se ha seleccionado ninguno, lanza un excepción
    GtkListBoxRow *row = 

    // Retorna el nombre del widget seleccionado
	return gtk_bin_get_child( GTK_BIN( row ) );
}*/

void Widget::establecerBuildable( GtkBuilder *buildable ){
    this -> builder = buildable;
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
    builder = gtk_builder_new();

    // ¿Ocurre algún error al cargar el archivo?
    if( gtk_builder_add_from_file( builder, archivo.c_str(), &error ) == 0 ){
        // Lanza una excepcion
        throw runtime_error( "<- Widget::cargarWidget() Error al cargar el archivo " + archivo + "." ); 
    }
}

// Establece el icono de la ventana
void Widget::establecerIconoVentana( string idVentana, string archivo )
{
    GObject *objeto = obtenerObjeto( idVentana );

    gtk_window_set_icon_from_file( GTK_WINDOW( objeto ), archivo.c_str(), NULL );
}

void Widget::establecerNombreWidget( string idWidget, string nombreWidget )
{
    GObject *objeto = obtenerObjeto( idWidget );

    return gtk_widget_set_name( GTK_WIDGET( objeto ), nombreWidget.c_str() );
}

string Widget::obtenerNombreWidget( string idWidget )
{
    GObject *objeto = obtenerObjeto( idWidget );

    return gtk_widget_get_name( GTK_WIDGET( objeto ) );
}

// Obtiene el objeto del Widget de acuero al id indicado
GObject *Widget::obtenerObjeto( string id ) const
{
    return gtk_builder_get_object( builder, id.c_str() );
}

// Conecta la señal a una función
void Widget::conectarSenal( Signal &senal, GCallback funcion, gpointer data )
{
    // Si hay una señal asociada al mismo, la desconecta
    if( senal.id > 0 ){
        desconectarSenal( senal );
    }

    // Obtiene el objeto indicado por el ID
    GObject *object = obtenerObjeto( senal.object );

    // Conecta la señal dada y la función dada con ese objeto
    senal.id = g_signal_connect( object, senal.type.c_str(), funcion, data );
}

// Desconecta la señal dado el id correspondiente
void Widget::desconectarSenal( Signal &senal )
{
    // Obtiene el objeto indicado por la señal
    GObject *object = obtenerObjeto( senal.object );

    if( senal.id > 0 ){
        // Desconecta la señal
        g_signal_handler_disconnect( object, senal.id );
    }
    
    // Establece el identificador en cero
    senal.id = 0;
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

// Inserta el elemento de un Widget dado dentro de otro del uno de los elementos de este Widget
void Widget::insertarElementoListBox( const Widget *elemento, string idFuente, string idDestino, gint position )
{
    // ¿El Widget no está vacío?
    if( elemento != nullptr ){
        // Obtiene el objeto con idFuente y el objeto con idDestino
        GObject *objetoFuente = elemento -> obtenerObjeto( idFuente );
        GObject *objetoDestino = obtenerObjeto( idDestino );
        
        // Se agrega
        gtk_list_box_insert( GTK_LIST_BOX( objetoDestino ), GTK_WIDGET( objetoFuente ), position );
    }
}

// Obtiene el nombre del Widget seleccionado de un ListBox
std::string Widget::obtenerWidgetSeleccionadoListBox( std::string idListBox )
{
    // Obtiene el contenedor
    GObject *listBox = obtenerObjeto( idListBox );

    // Busca el renglón seleccionado y si no se ha seleccionado ninguno, lanza un excepción
    GtkListBoxRow *row = gtk_list_box_get_selected_row( GTK_LIST_BOX( listBox ) );
    if( row == NULL ){
        throw invalid_argument( "No se ha seleccionado ninguna opción." );
    }

    // Retorna el nombre del widget seleccionado
    GtkWidget *item = gtk_bin_get_child( GTK_BIN( row ) );
	return gtk_widget_get_name( item );
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
    
    // Se oculta el objeto
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

void Widget::habilitarEdicionEntrada( std::string idEntrada )
{
    GObject *objeto = obtenerObjeto( idEntrada );
    
    gtk_editable_set_editable( GTK_EDITABLE( objeto ), TRUE );
}

void Widget::deshabilitarEdicionEntrada( std::string idEntrada )
{
    GObject *objeto = obtenerObjeto( idEntrada );
    
    gtk_editable_set_editable( GTK_EDITABLE( objeto ), FALSE );
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

void Widget::establecerActivoBotonToggle( std::string idBoton )
{
    GObject *objeto = obtenerObjeto( idBoton );
    
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( objeto ), TRUE );
}

// Obtiene el estado de un toggle button (si esta activo o inactivo)
bool Widget::obtenerEstadoBotonToggle( std::string id )
{
    // Obtiene el objeto que necesita
    GObject *objeto = obtenerObjeto( id );
    
    return gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( objeto ) );
}

// Agrega una opcion al combobox text indicado
void Widget::agregarOpcionComboBoxText( std::string idComboBox, std::string opcion, const char *idOpcion )
{
    GObject *objeto = obtenerObjeto( idComboBox.c_str() );
    
    gtk_combo_box_text_append( GTK_COMBO_BOX_TEXT( objeto ), idOpcion, opcion.c_str() );
}

void Widget::establecerOpcionComboBox( std::string idComboBox, unsigned int indice )
{
    GObject *objeto = obtenerObjeto( idComboBox.c_str() );
    gtk_combo_box_set_active( GTK_COMBO_BOX( objeto ), indice );
}

// Establece la opcion con el id indicado como el objeto activo
void Widget::establecerActivoComboBoxText( std::string idComboBox, std::string idOpcion )
{
    GObject *objeto = obtenerObjeto( idComboBox.c_str() );
    
    gtk_combo_box_set_active_id( GTK_COMBO_BOX( objeto ), idOpcion.c_str() );
}

// Obtiene la opcion seleccionada
const char *Widget::obtenerOpcionComboBoxText( std::string idComboBox ){
    GObject *objeto = obtenerObjeto( idComboBox.c_str() );
    
    return gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( objeto ) );
}

// Limpia el combobox text indicado removiendo todas las opciones
void Widget::limpiarComboBoxText( std::string idComboBox ){
    GObject *objeto = obtenerObjeto( idComboBox.c_str() );
    
    gtk_combo_box_text_remove_all( GTK_COMBO_BOX_TEXT( objeto ) );
}

// Establece la fecha en el calendario
void Widget::establecerFechaCalendario( std::string idCalendario, unsigned int dia, unsigned int mes, unsigned int anio ){
    GObject *objeto = obtenerObjeto( idCalendario );

    // Establece el día y el mes
    gtk_calendar_select_day( GTK_CALENDAR( objeto ), dia );
    gtk_calendar_select_month( GTK_CALENDAR( objeto ), mes, anio );
}

// Obtiene la fecha de la entrada de calendario dada
void Widget::obtenerFechaCalendario( std::string idCalendario, unsigned int *dia, unsigned int *mes, unsigned int *anio ){
    // Obtiene el calendario
    GObject *objeto = obtenerObjeto( idCalendario );

    // Se asegura que el objeto exista
    gtk_calendar_get_date( GTK_CALENDAR( objeto ), anio, mes, dia );
}

// Permite cargar una imagen desde archivo para un GtkImage
void Widget::establecerImagen( string idImagen, string archivo ){
    GObject *objeto = obtenerObjeto( idImagen.c_str() );
    
    gtk_image_set_from_file( GTK_IMAGE( objeto ), archivo.c_str() );
}

// Obtiene el ancho de un Widget
unsigned int Widget::obtenerAnchoWidget( string idWidget ){
    GObject *objeto = obtenerObjeto( idWidget );

    return gtk_widget_get_allocated_width( GTK_WIDGET( objeto ) );
}

//  Obtiene el alto de un Widget
unsigned int Widget::obtenerAltoWidget( string idWidget )
{
    GObject *objeto = obtenerObjeto( idWidget );

    return gtk_widget_get_allocated_height( GTK_WIDGET( objeto ) );
}

void Widget::removerElementosHijos( std::string idContenedor ){
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

void Widget::establecerModeloVistaArbol( std::string idVistaArbol, GtkListStore *lista )
{
    GObject *objeto = obtenerObjeto( idVistaArbol );
    gtk_tree_view_set_model( GTK_TREE_VIEW( objeto ), GTK_TREE_MODEL( lista ) );
}

void Widget::destruir()
{
    gtk_widget_destroy( GTK_WIDGET( builder ) );
}
