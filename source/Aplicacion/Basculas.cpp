#include "Basculas.h"
#include "Aplicacion.h"
#include "Vistas.h"
#include <iostream>
#include <sstream>
using namespace std;

// Boton guardar basculas
Signal senal_bascula_registros_listar = { "OpcionBasculas", "clicked", 0 };
Signal senal_bascula_seleccionar{ "ContenedorBasculas", "row-activated", 0 };
Signal botonBasculaAgregar{ "BotonBasculaAgregar", "clicked", 0 };
Signal botonBasculaEditar{ "BotonBasculaEditar", "clicked", 0 };
Signal botonBasculaEliminar{ "BotonBasculaEliminar", "clicked", 0 };
Signal senal_bascula_guardar_nuevo{ "BotonGuardarBascula", "clicked", 0 };

void bascula_registros_listar( GtkWidget *widget, gpointer info ){
    cout << "bascula_registros_listar" << endl;

    // Obtiene las básculas registradas
    bascula_registros_actualizar();

    // Conecta las señales
    conectar_senal( senal_bascula_guardar_nuevo, G_CALLBACK( bascula_guardar_nuevo ), nullptr );
    conectar_senal( senal_bascula_seleccionar, G_CALLBACK( bascula_seleccionar ), nullptr );
    conectar_senal( botonBasculaAgregar, G_CALLBACK( vistaBasculaEdicion ), nullptr );
    conectar_senal( botonBasculaEliminar, G_CALLBACK( vistaBasculaEliminar ), nullptr );

    // Redirige a básculas
	irA( "Basculas", false );
}

void bascula_seleccionar( GtkListBox *box, GtkListBoxRow *row, gpointer data ){
    cout << "bascula_seleccionar" << endl;

    // Obtiene el folio de la bascula
    string folio = gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( row ) ) ) );

    // Obtiene la báscula
    database.open( databaseFile );
    database.query( "select * from Basculas where id_bascula = " + folio );
    database.close();

    // ¿Hay resultados?
    if( results.size() < 1 ){
        runtime_error( "Ocurrió un error al recuperar el registro con id " + folio );
    }
    
    // Obtiene el registo
    unordered_map< string, string > *bascula = results.at( 0 );

    // Establece las opciones que indica la bascula
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "BasculaNombre" ) ), (* bascula)[ "nombre" ].c_str() );
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "BasculaPuerto" ) ), (* bascula)[ "puerto" ].c_str() );
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "BasculaVelocidad" ) ), (* bascula)[ "baudrate" ].c_str() );
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "BasculaBitsDatos" ) ), (* bascula)[ "bytesize" ].c_str() );
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "BasculaBitsDetencion" ) ), (*bascula)[ "stopbits" ].c_str() );
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "BasculaIgnorarOctetos" ) ), (*bascula)[ "ignoredbytes" ].c_str() );

    // Redirige hacia la opcion de básculas
    irA( "Bascula", false );
}

/*
void vistaBascula( GtkListBox *box, GtkListBoxRow *row, gpointer data )
{
	// Obtiene la bascula seleccionada
	Bascula *bascula = basculaBuscarPorCodigo( obtenerFolioSelector( row ) );
	if( bascula == nullptr ){
		throw runtime_error( "Ocurrió un error al intentar recuperar la bascula seleccionada." );
	}

	// Establece las opciones que indica la bascula
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "OpcionesBasculaCodigo" ) ), to_string( obtenerFolioSelector( row ) ).c_str() );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaNombre" ) ), bascula -> obtenerNombre().c_str() );
	gtk_combo_box_set_active_id( GTK_COMBO_BOX( buscar_objeto( "OpcionesBasculaPuerto" ) ), bascula -> obtenerPuerto().c_str() );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBitsDatos" ) ), to_string( bascula -> obtenerByteSize() ).c_str() );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBitsStop" ) ), to_string( bascula -> obtenerStopBits() ).c_str() );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBytesIgnorados" ) ), to_string( bascula -> obtenerBytesIgnorados() ).c_str() );

	// Redirige hacia la vista
	irA( "BasculaEditar", false );
}
*/

void bascula_guardar_nuevo( GtkWidget *widget, gpointer info ){
    cout << "bascula_guardar_nuevo" << endl;

    try{
        // Obtiene los datos del formulario (valida)
        string nombre = bascula_nombre_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaNombre" ) ) ) );
        string puerto = bascula_puerto_validar( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "OpcionesBasculaPuerto" ) ) ) );
        int tasa_baudios = bascula_tasa_baudios_validar( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "OpcionesBasculaBaudrate" ) ) ) );
        int largo_octeto = bascula_largo_octeto_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBitsDatos" ) ) ) );
        int bits_detencion = bascula_bits_detencion_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBitsStop" ) ) ) );
        int paridad = bascula_paridad_validar( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "OpcionesBasculaParidad" ) ) ) );
        int octetos_ignorados = stoi( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBytesIgnorados" ) ) ) );

        // Agrega la báscula a la base de datos
        stringstream consulta;
        consulta << "insert into Basculas values ( null, '" << nombre << "', '" << puerto << "', "
                 << tasa_baudios << ", " << largo_octeto << ", " << paridad << ", " << bits_detencion << ", "
                 << octetos_ignorados << ", " << 0 << " )";

        // Muestra los datos obtenidos
        cout << consulta.str() << endl;
        
        //
        database.open( databaseFile );
        database.query( consulta.str() );
        database.close();

        // Actualiza la lista de básculas registradas
        bascula_registros_actualizar();

        // Regresa
        regresarVista();

        // Muestra el mensaje de éxito
        app_mostrar_exito( "Báscula registrada correctamente." );
    }
    catch( invalid_argument &ia ){
        string what = ia.what();
        app_mostrar_error( what );
        if( what.compare( "stoi" ) == 0 ){
            app_mostrar_error( "Introduzca un valor numérico válido." );
        }    
    }
}

void bascula_registros_actualizar( void ){
    // Limpia el contenedor de básculas
    limpiar_contenedor( "ContenedorBasculas" );

    // Indica que no has básculas registradas
    gtk_widget_show( GTK_WIDGET( buscar_objeto( "MensajeBasculas" ) ) );

    // Obtiene las básculas registradas
    database.open( databaseFile );
    database.query( "select * from Basculas" );
    database.close();
    if( results.size() > 0 ){
        gtk_widget_hide( GTK_WIDGET( buscar_objeto( "MensajeBasculas" ) ) );
        for( unordered_map< string, string > *bascula : results ){
            // Crea un elemento que será añadido a la interfaz
            GtkBuilder *builder = gtk_builder_new();
            GError *error = NULL;

            if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoBascula.glade", &error ) != 0 ){
                gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "ItemBascula" ) ), (*bascula)[ "id_bascula" ].c_str() );
                gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemBasculaNombre" ) ), (* bascula)[ "nombre" ].c_str() );
                gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemBasculaPuerto" ) ), (* bascula)[ "puerto" ].c_str() );
                gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemBasculaVelocidad" ) ), ( (* bascula)[ "baudrate" ] + " bps" ).c_str() );
            }

            // Lo agrega al contenedor y la lista de basculas registradas
            gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorBasculas" ) ), GTK_WIDGET( gtk_builder_get_object( builder, "ItemBascula" ) ), stoi( (* bascula)[ "id_bascula" ] ) );
        }
    }
}

int velocidades_tasa_baudios[] = { 110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000, 256000 };

string bascula_nombre_validar( string nombre ){
    // Formato que contiene la expresion regular
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ,\\s.]*" );

    // Verifica que nombre no sea mayor que cincuenta caracteres
    if( nombre.empty() ){
        throw invalid_argument( "Es necesario un identificador de la báscula a la que corresponde." );
    }

    // Verifica que nombre no sea mayor que cincuenta caracteres
    if( nombre.size() > 50 ){
        throw invalid_argument( "El identificador establecido excede el tamaño máximo de caracteres. (50)" );
    }

    // Verifica el nombre
    if( !regex_match( nombre, formato ) ){
		throw invalid_argument( "Se está estableciendo un identificador no válido." );
	}

    return nombre;
}

string bascula_puerto_validar( string puerto ){
    if( puerto.compare( "Seleccionar" ) == 0 ){
        throw invalid_argument( "Seleccione un puerto." );
    }

    return puerto;
}

int bascula_tasa_baudios_validar( string ratio ){
    // Obtiene el valor del baudrate seleccionado
    int valor = stoi( ratio );

    // Verifica que sea alguno de los valores válidos
    for( int velocidad : velocidades_tasa_baudios ){
        if( valor == velocidad ){
            return valor;
        }
    }

    throw invalid_argument( "La tasa de baudios establecida no es válida." );
}

int bascula_largo_octeto_validar( string tamanio ){
    // Obtiene el valor dado
    int valor = stoi( tamanio );

    // Verifica que se encuentre en el rango apropiado
    if( valor < 7 || valor > 8 ){
        throw invalid_argument( "Introduzca un tamaño de octeto válido." );
    }

    return valor;
}

int bascula_bits_detencion_validar( string bits ){
    // Obtiene el valor
    int valor = stoi( bits );

    // Verifica que se encuentre en un valor apropiado
    if( valor < 0 || valor > 2 ){
        throw invalid_argument( "Introduzca una cantidad válida para los bits de detención." );
    }

    // Retorna el valor correctamente validado
    return valor;
}

int bascula_paridad_validar( string paridad ){
    // Obtiene el valor
    if( paridad.compare( "Ninguno" ) == 0 ){
        return 0; // No paridad
    }
    else if( paridad.compare( "Par" ) == 0 ){
        return 2; // Par 
    }
    else if( paridad.compare( "Impar" ) == 0 ){
        return 1; // Impar
    }
    else if( paridad.compare( "Marca" ) == 0 ){
        return 3; // Marca
    }
    else if( paridad.compare( "Espacio" ) == 0 ){
        return 4; // Espacio
    }
    else{
        throw invalid_argument( "La paridad que se desea establecer no es una opción válida." );
    }

    return 0;
}