#include "Basculas.h"
#include "Aplicacion.h"
#include "Funciones.h"
#include "Senales.h"
#include "Vistas.h"
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <ctime>
using namespace std;

// Hilo para leer el peso
thread *hilo;

// Manejador
bool activo;
HANDLE manejador;
int id_bascula;
bool bascula_esperando = false;
bool bascula_entrada_manual_habilitado = false;

struct Source{
    GSource *source;
    guint source_id;
};

Source fuente_lector = { NULL, 0 };

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

// Actividades
Signal senal_bascula_guardar{ "BotonGuardarBascula", "clicked", 0 };

Signal senal_bascula_registros_listar = { "OpcionBasculas", "clicked", 0 };
Signal senal_bascula_seleccionar = { "ContenedorBasculas", "row-activated", 0 };
Signal senal_bascula_nuevo = { "BotonBasculaAgregar", "clicked", 0 };
Signal senal_bascula_editar = { "BotonBasculaEditar", "clicked", 0 };
Signal senal_bascula_eliminar = { "BotonBasculaEliminar", "clicked", 0 };

// Señales de la ventana de registro de peso
Signal senal_opcion_seleccionar = { "BasculasRegistradas", "changed", 0 };
Signal senal_bascula_registrar_pesaje = { "BotonRegistrarLectura", "clicked", 0 };
Signal senal_bascula_cancelar_pesaje = { "BotonCancelarLectura", "clicked", 0 };

void bascula_registros_listar( GtkWidget *widget, gpointer info ){
    cout << "bascula_registros_listar" << endl;

    // Obtiene las básculas registradas
    bascula_registros_actualizar();

    // Conecta las señales
    conectar_senal( senal_bascula_nuevo, G_CALLBACK( bascula_nuevo ), nullptr );
    conectar_senal( senal_bascula_editar, G_CALLBACK( bascula_editar ), nullptr );
    conectar_senal( senal_bascula_seleccionar, G_CALLBACK( bascula_seleccionar ), nullptr );
    conectar_senal( botonSi, G_CALLBACK( bascula_eliminar ), nullptr );
    conectar_senal( senal_bascula_eliminar, G_CALLBACK( app_alerta ), (void *)"¿Deseas eliminar la configuración de esta báscula?" );

    // Redirige a básculas
	irA( "Basculas", false );
}

void bascula_nuevo( GtkWidget *widget, gpointer data ){
    // Limpia el formulario
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "BasculaNombreEditar" ) ), "" );
    gtk_combo_box_set_active( GTK_COMBO_BOX( buscar_objeto( "BasculaPuertoEditar" ) ), 0 );
    gtk_combo_box_set_active( GTK_COMBO_BOX( buscar_objeto( "BasculaBaudrateEditar" ) ), 6 );
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "BasculaBitsDatosEditar" ) ), "8" );
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "BasculaBitsStopEditar" ) ), "0" );
    gtk_combo_box_set_active( GTK_COMBO_BOX( buscar_objeto( "BasculaParidadEditar" ) ), 0 );
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "BasculaBytesIgnoradosEditar" ) ), "0" );

    // Conecta la señal de guardado
    conectar_senal( senal_bascula_guardar, G_CALLBACK( bascula_guardar_nuevo ), nullptr );

    // Redirige hacia la vista
    irA( "BasculaEditar", false );
}

void bascula_editar( GtkWidget* widget, gpointer data ){
	// Obtiene la bascula seleccionada
    GtkListBoxRow *row = gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorBasculas" ) ) );

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

    //
    unordered_map< string, string > *bascula = results.at( 0 );

	// Establece las opciones que indica la bascula
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "BasculaNombreEditar" ) ), (* bascula)[ "nombre" ].c_str() );
	gtk_combo_box_set_active_id( GTK_COMBO_BOX( buscar_objeto( "BasculaPuertoEditar" ) ), (* bascula)[ "puerto" ].c_str() );
    gtk_combo_box_set_active_id( GTK_COMBO_BOX( buscar_objeto( "BasculaBaudrateEditar" ) ), (* bascula)[ "baudrate" ].c_str() );
    gtk_combo_box_set_active_id( GTK_COMBO_BOX( buscar_objeto( "BasculaParidadEditar" ) ), bascula_paridad_leer_opcion( (* bascula)[ "parity" ] ) );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "BasculaBitsDatosEditar" ) ), (* bascula)[ "bytesize" ].c_str() );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "BasculaBitsStopEditar" ) ), (* bascula)[ "stopbits" ].c_str() );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "BasculaBytesIgnoradosEditar" ) ), (* bascula)[ "ignoredbytes" ].c_str() );

    //
    conectar_senal( senal_bascula_guardar, G_CALLBACK( bascula_guardar_edicion ), nullptr );

	// Redirige hacia la vista
	irA( "BasculaEditar", false );
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
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "BasculaParidad" ) ), bascula_paridad_leer_opcion( (* bascula)[ "parity" ] ) );
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "BasculaIgnorarOctetos" ) ), (*bascula)[ "ignoredbytes" ].c_str() );

    // Redirige hacia la opcion de básculas
    irA( "Bascula", false );
}

void bascula_guardar_nuevo( GtkWidget *widget, gpointer info ){
    cout << "bascula_guardar_nuevo" << endl;

    try{
        // Obtiene los datos del formulario (valida)
        string nombre = bascula_nombre_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "BasculaNombreEditar" ) ) ) );
        string puerto = bascula_puerto_validar( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculaPuertoEditar" ) ) ) );
        int tasa_baudios = bascula_tasa_baudios_validar( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculaBaudrateEditar" ) ) ) );
        int largo_octeto = bascula_largo_octeto_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "BasculaBitsDatosEditar" ) ) ) );
        int bits_detencion = bascula_bits_detencion_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "BasculaBitsStopEditar" ) ) ) );
        int paridad = bascula_paridad_validar( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculaParidadEditar" ) ) ) );
        int octetos_ignorados = stoi( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "BasculaBytesIgnoradosEditar" ) ) ) );

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

void bascula_guardar_edicion( GtkWidget *widget, gpointer info ){
    cout << "bascula_guardar_nuevo" << endl;

    try{
        // Obtiene la bascula seleccionada
        GtkListBoxRow *row = gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorBasculas" ) ) );

	    // Obtiene el folio de la bascula
        string folio = gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( row ) ) ) );

        // Obtiene los datos del formulario (valida)
        string nombre = bascula_nombre_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "BasculaNombreEditar" ) ) ) );
        string puerto = bascula_puerto_validar( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculaPuertoEditar" ) ) ) );
        int tasa_baudios = bascula_tasa_baudios_validar( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculaBaudrateEditar" ) ) ) );
        int largo_octeto = bascula_largo_octeto_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "BasculaBitsDatosEditar" ) ) ) );
        int bits_detencion = bascula_bits_detencion_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "BasculaBitsStopEditar" ) ) ) );
        int paridad = bascula_paridad_validar( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculaParidadEditar" ) ) ) );
        int octetos_ignorados = stoi( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "BasculaBytesIgnoradosEditar" ) ) ) );

        // Agrega la báscula a la base de datos
        stringstream consulta;
        consulta << "update Basculas set nombre = '" << nombre << "', puerto = '" << puerto 
                 << "', baudrate = " << tasa_baudios << ", bytesize = " << largo_octeto << ", parity = " << paridad 
                 << ", stopbits = " << bits_detencion << ", ignoredbytes = " << octetos_ignorados << " where id_bascula = " << folio;
        
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

void bascula_eliminar( GtkWidget *widget, gpointer info ){
    // Oculta la alerta
    gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );

    // Obtiene la bascula seleccionada
    GtkListBoxRow *row = gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorBasculas" ) ) );

	// Obtiene el folio de la bascula
    string folio = gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( row ) ) ) );

    // Obtiene la báscula
    database.open( databaseFile );
    database.query( "delete from Basculas where id_bascula = " + folio );
    database.close();

    // Actualiza la lista de registros
    bascula_registros_actualizar();

    // Muestra el mensaje de exito
    app_mostrar_exito( "Bascula eliminada correctamente." );
}

void bascula_lector_abrir( GtkWidget *widget, gpointer info ){
    cout << "bascula_lector_abrir" << endl;
    
    // Establece el lector en ceros
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaPeso" ) ), "0.00 Kg" );

    // Limpia las básculas que había y agrega la opcion Seleccionar
    gtk_combo_box_text_remove_all( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculasRegistradas" ) ) );
    gtk_combo_box_text_append( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculasRegistradas" ) ), "Seleccionar", "Seleccionar" );
    gtk_combo_box_text_append( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculasRegistradas" ) ), "Manual", "Entrada manual" );
    gtk_combo_box_set_active( GTK_COMBO_BOX( buscar_objeto( "BasculasRegistradas" ) ), 0 );

    // Obtiene las básculas disponibles
    database.open( databaseFile );
    database.query( "select * from Basculas" );
    database.close();
    if( results.size() > 0 ){
        for( unordered_map< string, string > *opcion : results ){
            gtk_combo_box_text_append( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculasRegistradas" ) ), (* opcion)[ "id_bascula" ].c_str(), (* opcion)[ "nombre" ].c_str() );
        }
    }

    // Conecta la señal
    conectar_senal( senal_opcion_seleccionar, G_CALLBACK( bascula_lector_seleccionar ), nullptr );
    conectar_senal( senal_bascula_registrar_pesaje, G_CALLBACK( bascula_registrar_pesaje ), (char *)"EntradaHoraEntradaInterno EntradaPesoBrutoInterno" );
    conectar_senal( senal_bascula_cancelar_pesaje, G_CALLBACK( bascula_lector_cerrar ), nullptr  );

    // Abre la ventana
    gtk_widget_show( GTK_WIDGET( buscar_objeto( "VentanaLectorPeso" ) ) );
}

void bascula_lector_seleccionar( GtkComboBox *lista, gpointer info ){
    cout << "bascula_lector_seleccionar" << endl;
    // Obtiene la opcion seleccionada
    string opcion = gtk_combo_box_get_active_id( lista );

    // Verifica que no exista un thread abierto
    bascula_lector_finalizar();

    // Revisa que no sea opción manual o seleccionar
    if( opcion.compare( "Seleccionar" ) == 0 ){
        return;
    }
    else if( opcion.compare( "Manual" ) == 0 ){
        return;
    }

    try{
        // Obtiene el id de la báscula
        id_bascula = stoi( opcion );

        // Comenza la lectura
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "EstadoLectura" ) ), "Conectando" );
        
        // Obtiene los datos de la bascula a conectar
        database.open( databaseFile );
        database.query( "select * from Basculas where id_bascula = " + to_string( id_bascula ) );
        database.close();

        // ¿Opción no existente?
        if( results.size() < 1 ){
            throw invalid_argument( "La opción seleccionada es inválida. Intente otra opción." );
        }

        // Obtiene el primer resultado
        unordered_map< string, string > *bascula = results.at( 0 );

        // Conecta a la báscula
        activo = bascula_lector_iniciar( bascula );

        // Indica que está leyendo
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "EstadoLectura" ) ), "Leyendo" );

        // Establece para actualizar el peso
        source_conectar( fuente_lector, G_SOURCE_FUNC( bascula_lector_leer ) );

        // Crea el hilo que monitorea el peso
        bascula_esperando = false;
        hilo = new thread( bascula_perro_guardian, 1000 );
    }
    catch( invalid_argument &ia ){
        string what = ia.what();
        app_mostrar_error( what );
        if( what.compare( "stoi" ) == 0 ){
            app_mostrar_error( "Opción no válida o inexistente." );
        }
    }
}

bool bascula_lector_leer( gpointer info ){
    cout << "bascula_lector_leer" << endl;
    // Obtiene los datos de la bascula a conectar
    database.open( databaseFile );
    database.query( "select * from Basculas where id_bascula = " + to_string( id_bascula ) );
    database.close();

    // ¿Opción no existente?
    if( results.size() < 1 ){
        throw invalid_argument( "La opción seleccionada es inválida. Intente otra opción." );
    }

    // Obtiene el primer resultado
    unordered_map< string, string > *bascula = results.at( 0 );

    // Obtiene el objeto que se necesita bloquear
    GtkLabel *etiquetaPeso = GTK_LABEL( buscar_objeto( "EtiquetaPeso" ) );

    // Comienza a actualizar los datos de la báscula
    if( activo ){
        BOOL status = FALSE;
        DWORD noBytesRead;
        char bufferRead[ 256 ], buffer[ 256 ] = "";
        char tempChar = ' ';
        unsigned int i = 0;

        do{
            status = ReadFile( manejador, &tempChar, sizeof( tempChar ), &noBytesRead, NULL );
            if( tempChar != '\r' ){
                bufferRead[ i ] = tempChar;
                ++i;
            }
        }while( tempChar != '\n' && status == TRUE && i < 256 );

        if( status == TRUE ){
            size_t j = stoi( (* bascula)[ "ignoredbytes"] );
            size_t k = 0;

            while( j < i  && j < 256 && k < 256 ){
                buffer[ k ] = bufferRead[ j ];
                ++j;
                ++k;
                buffer[ k ] = '\0';
            }

            // Convierte el peso leído a un número double
            string peso;
            try{
                peso = pesoString( stod( buffer ), 2, true );
            }
            catch( invalid_argument &ia ){
                peso = "0.00 Kg";
            }
            gtk_label_set_text( etiquetaPeso, peso.c_str() );

            // Establece que ya no está esperando entrada
            bascula_esperando = false;
        }
    }

    return activo;
}

bool bascula_lector_iniciar( unordered_map< string, string > *bascula ){
    // Intenta crear el manejador
    manejador = CreateFile( (* bascula)[ "puerto" ].c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
    if( manejador == INVALID_HANDLE_VALUE ){
		throw invalid_argument( "Ocurrió un error al conectarse al puerto especificado." );
	}

    // Crea una estructura con las opciones de configuración serial y establece los parámetros correspondientes
    DCB serial_parametros = { 0 };
    serial_parametros.DCBlength = sizeof( serial_parametros );
    serial_parametros.BaudRate = (DWORD)stoi( (* bascula)[ "baudrate" ] );
	serial_parametros.ByteSize = (BYTE)stoi( (* bascula)[ "bytesize" ] );
	serial_parametros.StopBits = (BYTE)stoi( (* bascula)[ "stopbits" ] );
	serial_parametros.Parity = (BYTE)stoi( (* bascula)[ "parity" ] );
    
    // Acopla la configuración serial al manejador
    BOOL status = SetCommState( manejador, &serial_parametros );
	if( status == FALSE ){
		throw invalid_argument( "Hubo un error al intentar establecer los parámetros de configuración." );
	}

    // Establece la configuración de tiempos de espera
    COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 1;
	timeout.ReadTotalTimeoutConstant = 0;
	timeout.ReadTotalTimeoutMultiplier = 0;
	status = SetCommTimeouts( manejador, &timeout );
	if( status == FALSE ){
		throw runtime_error( "Error al establecer la información de tiempos de espera.");
	}

    // Solo permite lectura
    status = SetCommMask( manejador, EV_RXCHAR );
	if( status == FALSE ){
		throw runtime_error( "Error al configurar los parámetros de recepción.");
	}

    // Finalizó correctamente
    return true;
}

void bascula_lector_finalizar( void ){
    // Desactiva la báscula seleccionada
    activo = false;
    bascula_esperando = true;   // Le hacemos creer que sigue esperando al perro guardian
    source_desconectar( fuente_lector );
    CancelIoEx( manejador, NULL );
    CancelSynchronousIo( manejador );
    CloseHandle( manejador );
}

void bascula_registrar_pesaje( GtkWidget *widget, gpointer info ){
    try{
        // Estream para obtener de forma separada las etiquetas
        stringstream stream;
        string etiquetaHora;
        string etiquetaPeso;
        string peso;

        // Detiene el lector
        bascula_lector_finalizar();

        // Si hay un hilo en ejecución lo cierra
        if( hilo != nullptr ){
            hilo -> join();
            hilo = nullptr;
        }
        
        // Carga los datos en el stream
        stream << (char *)info;
        
        // Obtiene la etiqueta hora
        stream >> etiquetaHora;
        stream >> etiquetaPeso;

        // Obtiene el texto de l
        if( bascula_entrada_manual_habilitado ){
            peso  = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaPeso" ) ) );
        }
        else{
            peso = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EtiquetaPeso" ) ) );
        }

        // Registra la hora y el peso
        gtk_label_set_text( GTK_LABEL( buscar_objeto( etiquetaHora ) ), tiempo_leer_hora( 1 ).c_str() );
        gtk_label_set_text( GTK_LABEL( buscar_objeto( etiquetaPeso ) ), peso.c_str() );

        // Cierra la ventana
        gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaLectorPeso" ) ) );   
    }
    catch( exception e ){
        cout << e.what() << endl;
    }
}

gboolean bascula_lector_cerrar( GtkWidget *widget, gpointer info ){
    // Oculta la ventana
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaLectorPeso" ) ) );

    // Desconecta la señal de seleccion de bascula
    desconectar_senal( senal_opcion_seleccionar );
    
    bascula_lector_finalizar();

    if( hilo != nullptr ){
        hilo -> join();
        hilo = nullptr;
    }

    return TRUE;
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

void bascula_perro_guardian( unsigned int milisegundos ){
    cout << "perro_guardian" << endl;
    // Mientras no espera entrada
    do{
        bascula_esperando = true;
        std::this_thread::sleep_for( std::chrono::milliseconds( milisegundos ) );
    }while( !bascula_esperando );

    // Excedio el limite y nunca llego valor
    if( bascula_esperando && activo ){
        gtk_combo_box_set_active( GTK_COMBO_BOX( buscar_objeto( "BasculasRegistradas" ) ), 0 );
        bascula_lector_finalizar();
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "EstadoLectura" ) ), "Tiempo de espera agotado." );
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
    if( valor < 5 || valor > 8 ){
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

const char *bascula_paridad_leer_opcion( string paridad ){
    switch( stoi( paridad ) ){
        case 0:
            return "Ninguno";
            break;
        case 1:
            return "Impar";
            break;
        case 2:
            return "Par";
            break;
        case 3:
            return "Marca";
            break;
        case 4:
            return "Espacio";
            break;
        default:
            throw runtime_error( "Valor que se intenta establecer no es válido." );
            break;
    }

    return "";
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