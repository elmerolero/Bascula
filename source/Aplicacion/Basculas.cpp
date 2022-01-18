#include "Basculas.h"
#include <iostream>
using namespace std;

// Boton guardar basculas
Signal senal_bascula_guardar_nuevo{ "BotonGuardarBascula", "clicked", 0 };

void bascula_registros_actualizar( void ){
    // Limpia el contenedor de básculas
    limpiar_contenedor( "ContenedorBasculas" );

    // Obtiene las básculas registradas
    database.open( databaseFile );
    database.query( "select * from Basculas" );
    database.close();

    if( results.size() > 0 ){
        for( unordered_map< string, string > *bascula : results ){
            // Crea un elemento que será añadido a la interfaz
            GtkBuilder *builder = gtk_builder_new();
            GError *error = NULL;

            if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoBascula.glade", &error ) != 0 ){
                gtk_widget_set_name( GTK_WIDGET( buscar_objeto( "ItemBascula" ) ), (* bascula)[ "id_bascula" ].c_str() );
                gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemBasculaNombre" ) ), (* bascula)[ "nombre" ].c_str() );
                gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemBasculaPuerto" ) ), (* bascula)[ "puerto" ].c_str() );
                gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemBasculaVelocidad" ) ), ( (* bascula)[ "baudrate" ] + " bps" ).c_str() );
            }

            // Lo agrega al contenedor y la lista de basculas registradas
            gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorBasculas" ) ), GTK_WIDGET( gtk_builder_get_object( builder, "ItemBascula" ) ), stoi( (* bascula)[ "id_bascula" ] ) );
        }
    }
}

void bascula_guardar_nuevo( GtkWidget *widget, gpointer info ){
    cout << "bascula_guardar_nuevo" << endl;

    // Obtiene los datos del formulario
    string nombre = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaNombre" ) ) );
    string puerto = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "OpcionesBasculaPuerto" ) ) );
    string tasa_baudios = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "OpcionesBasculaBaudrate" ) ) );
    string largo_caracter = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBitsDatos" ) ) );
    string bitsAlto = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBitsStop" ) ) );
    string paridad = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "OpcionesBasculaParidad" ) ) );
    string octetos_ignorados = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBytesIgnorados" ) ) );

    // Muestra los datos obtenidos
    cout << "Nombre:\t" << nombre << "\n"
         << "Puerto:\t" << puerto << "\n"
         << "Tasa de baudios:\t" << tasa_baudios << "\n"
         << "Largo caracter:\t" << largo_caracter << "\n"
         << "Bits de alto:\t" << bitsAlto << "\n"
         << "Paridad:\t" << paridad << "\n"
         << "Octetos ignorados:\t" << octetos_ignorados << endl;
}

string bascula_nombre_validar( string nombre );{
    // Formato que contiene la expresion regular
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ,\\s.]*" );

    // Verifica que nombre no sea mayor que cincuenta caracteres
    if( nombre.size() > 50 ){
        throw invalid_argument( "El identificador establecido excede el tamaño máximo de caracteres. (50)" );
    } || !regex_match( observaciones, formato ) ){
		throw invalid_argument( "Se están dando observaciones no válidas." );
	}
}
