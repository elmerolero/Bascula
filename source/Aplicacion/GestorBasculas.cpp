#include "GestorBasculas.h"
#include "Senales.h"
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <thread>
#include "Database.h"
using namespace std;

// Lista de las básculas creadas
unsigned int codigoBasculaActual;
list< Bascula * > basculasRegistradas;
Bascula *bascula;
thread *lectura;

void basculaAbrirLector()
{
    // Muestra la ventana
    gtk_widget_show( GTK_WIDGET( buscar_objeto( "VentanaLectorPeso" ) ) );
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "EstadoLectura" ) ), "Esperando selección" );

    // Establece el peso en 0 Kg
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaPeso" ) ), "0 Kg" );

     // Conecta las señales convenientes
    //coxBasculasRegistradas, G_CALLBACK( basculaLecturaIniciar ), nullptr );
}

gboolean basculaCerrarLector(){
    // Oculta la ventana
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaLectorPeso" ) ) );

    // Desconecta la señal de seleccion de bascula
    //desconectar_senal( comboBoxBasculasRegistradas );
    
    // Desactiva la báscula seleccionada
    if( bascula != nullptr ){
        bascula -> establecerActivo( false );
    }

    return TRUE;
}

void basculaLecturaIniciar()
{
    bascula = basculaBuscarPorCodigo( gtk_combo_box_get_active( GTK_COMBO_BOX( buscar_objeto( "BasculasRegistradas" ) ) ) );
    if( bascula != nullptr ){
        lectura = new thread( basculaLecturaActualizar );
    }
}

void basculaLecturaActualizar()
{   
    // Establece el estado del lector
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "EstadoLectura" ) ), ( "Conectando a " + bascula -> obtenerPuerto() ).c_str() );
    
    // Conecta con la báscula
    bascula -> conectar();
    
    // Indica que está leyendo
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "EstadoLectura" ) ), "Leyendo" );
    
    // Comienza a actualizar los datos de la báscula
    while( bascula -> estaActivo() ){
        // Actualiza el peso actual leído
        bascula -> actualizarPeso();

        // Lee el peso del lector
        double peso = bascula -> leer();

		// Establece el peso en la etiqueta
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaPeso" ) ), pesoString( peso, 2, true ).c_str() );
    }
    
    // Desconecta de la bascula
    bascula -> desconectar();
}

// Obtiene las básculas registradas en la base de datos
void basculaObtenerRegistros(){
    cout << "bascula_obtener_registros" << endl;

    try{
        // Obtiene las básculas registradas en la base de datos
        string consulta = "select * from basculas";
        database.open( databaseFile );
        database.query( consulta );
        database.close();
        
        if( results.size() > 0 ){ 
            for( auto *renglon : results ){
                // Crea el objeto para ese registro
                Bascula *bascula = new Bascula( renglon );

                // Lo añade al grupo de básculas
                basculasRegistradas.push_back( bascula );
            }
        }
        
        // Obtiene el codigo máximo registrado
        consulta = "select max( id_bascula ) as clave from basculas";
        database.open( databaseFile );
        database.query( consulta );
        database.close();

        // Actualiza la vista de básculas registradas
        basculaActualizarRegistros();
    }
    catch( runtime_error re ){
        throw re;
    }
}

// Obtiene todos los puertos disponibles
void basculaObtenerPuertosDisponibles()
{
	// Limpia las opciones que había anteriormente
    limpiar_contenedor( "BasculaPuertoEditar" );
	
	// Agrega las opciones seleccionar y las establece como por defecto
    gtk_combo_box_text_append( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculaPuertoEditar" ) ), "Seleccionar", "Seleccionar" );
	gtk_combo_box_set_active_id( GTK_COMBO_BOX( buscar_objeto( "BasculaPuertoEditar" ) ), "Seleccionar" );

	// Agrega los puertos seriales disponibles
	char directorioPuerto[ 5000 ];

	// Revisa si hay puertos disponibles
	int contadorDispositivos = 0;
	for( unsigned contador = 0; contador < 255; ++contador ){
		string nombrePuerto = "COM" + to_string( contador );
		DWORD intento = QueryDosDevice( nombrePuerto.c_str(), directorioPuerto, 5000 );
		if( intento != 0 ){
            gtk_combo_box_text_append( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculaPuertoEditar" ) ), ( contador > 9 ? "\\\\.\\" + nombrePuerto : nombrePuerto ).c_str(), ( contador > 9 ? "\\\\.\\" + nombrePuerto : nombrePuerto ).c_str() );
			contadorDispositivos++;
		}
	}

	if( contadorDispositivos < 1 ){
        app_mostrar_mensaje( "No se detectaron básculas." );
	}
}

void basculaGuardar()
{
    try{
        // Bandera que indica si es un registro nuevo o un registro existente
        bool nuevaBascula = false;

        // Crea la nueva báscula a agregar
        Bascula *bascula = basculaBuscarPorCodigo( stoi( gtk_label_get_text( GTK_LABEL( buscar_objeto( "OpcionesBasculaCodigo" ) ) ) ) );
        if( bascula == nullptr ){
            // Es un nuevo registro
            bascula = new Bascula();

            // Establece el código
            bascula -> establecerCodigo( ++codigoBasculaActual );
            nuevaBascula = true;
        }

        // Establece los datos de configuración establecidos
        bascula -> establecerNombre( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaNombre" ) ) ) );
        bascula -> establecerPuerto( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "OpcionesBasculaPuerto" ) ) ) );
        bascula -> establecerBaudRate( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "OpcionesBasculaBaudrate" ) ) ) );
        bascula -> establecerByteSize( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculasBitsDatos" ) ) ) );
        bascula -> establecerStopBits( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBitsStop" ) ) ) );
        bascula -> establecerParity( gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT( buscar_objeto( "OpcionesBasculaParidad" ) ) ) );
        bascula -> establecerBytesIgnorados( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBytesIgnorados" ) ) ) );

        // ¿Es nueva bascula?
        if( nuevaBascula ){
            basculaNuevo( bascula );
        }

        // Actualiza la lista de básculas registradas
        basculaActualizarRegistros();

        // Regresa
        regresarVista();

        // Indica que se añadió la báscula correctamente
        app_mostrar_error( "Bascula agregada correctamente." );
    }
    catch( invalid_argument &ia ){
        --codigoBasculaActual;
        app_mostrar_error( ia.what() );
    }
 
}

void basculaNuevo( Bascula *bascula )
{
    try{
        // Agrega la información a la base de datos si llegó aquí
        stringstream consulta;
        consulta << "insert into basculas values( "
                 << bascula -> obtenerCodigo() << ", '" 
                 << bascula -> obtenerNombre() << "', '"
                 << bascula -> obtenerPuerto() << "', "
                 << bascula -> obtenerBaudRate() << ", "
                 << (int)bascula -> obtenerByteSize() << ", "
                 << (int)bascula -> obtenerParity() << ", "
                 << (int)bascula -> obtenerStopBits() << ", "
                 << bascula -> obtenerBytesIgnorados() << ", 0 )"; 

        database.open( databaseFile );
        database.query( consulta.str() );
        database.close();

        // Agrega la báscula a la lista de básculas
        basculasRegistradas.push_back( bascula );
    }
    catch( invalid_argument &ia ){
        app_mostrar_error( ia.what() );
    }
}

void basculaEditar( Bascula *bascula )
{
    try{
        // Agrega la información a la base de datos si llegó aquí
        stringstream consulta;
        consulta << "update basculas set " 
                 << "nombre = '" << bascula -> obtenerNombre() << "', "
                 << "puerto = '" << bascula -> obtenerPuerto() << "', "
                 << "baudrate = " << bascula -> obtenerBaudRate() << ", "
                 << "bytesize = " << (int)bascula -> obtenerByteSize() << ", "
                 << "parity = " << (int)bascula -> obtenerParity() << ", "
                 << "stopbits = " << (int)bascula -> obtenerStopBits() << ", "
                 << "ignoredbyte = " << bascula -> obtenerBytesIgnorados() << ", 0 )"; 

        database.open( databaseFile );
        database.query( consulta.str() );
        database.close();
    }
    catch( invalid_argument &ia ){
        app_mostrar_error( ia.what() );
    }
}

void basculaEliminar()
{
    try{
        // Busca la bascula a eliminar
        Bascula *bascula = basculaBuscarPorCodigo( stoi( gtk_widget_get_name( gtk_bin_get_child( GTK_BIN( gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorBasculas" ) ) ) ) ) ) ) );
        if( bascula == nullptr ){
            throw runtime_error( "Ocurrió un error al recuperar un ticket seleccionado." );
        }

        // Agrega la información a la base de datos si llegó aquí
        stringstream consulta;
        consulta << "delete from basculas where id_bascula = " << bascula -> obtenerCodigo() ; 

        database.open( databaseFile );
        database.query( consulta.str() );
        database.close();

        // Elimina la báscula de la lista
        basculasRegistradas.remove( bascula );
        delete bascula;
        bascula = nullptr;

        // Actualiza la lista de básculas registradas
        basculaActualizarRegistros();

        // Indica que se añadió la báscula correctamente
        app_mostrar_error( "Báscula eliminada correctamente." );
    }
    catch( invalid_argument &ia ){
        app_mostrar_error( ia.what() );
    }
}

void basculaActualizarRegistros()
{
    try{
        // Limpia los contenedores
        limpiar_contenedor( "ContenedorBasculas" );
        gtk_combo_box_text_remove_all( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculasRegistradas" ) ) );
        gtk_combo_box_text_append( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculasRegistradas" ) ), NULL, "Seleccionar" );
        gtk_combo_box_set_active( GTK_COMBO_BOX( buscar_objeto( "BasculasRegistradas" ) ), 0 );

        // Para cada una de las básculas registradas
        for( Bascula *bascula : basculasRegistradas ){
            // Crea un elemento que será añadido a la interfaz
            GtkBuilder *builder = gtk_builder_new();
            GError *error = NULL;

            if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoBascula.glade", &error ) != 0 ){
                gtk_widget_set_name( GTK_WIDGET( buscar_objeto( "ItemBascula" ) ), to_string( bascula -> obtenerCodigo() ).c_str() );
                gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemBasculaNombre" ) ), bascula -> obtenerNombre().c_str() );
                gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemBasculaPuerto" ) ), bascula -> obtenerPuerto().c_str() );
                gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemBasculaVelocidad" ) ), ( to_string( bascula -> obtenerBaudRate() ) + " bps" ).c_str() );
            }

            // Lo agrega al contenedor y la lista de basculas registradas
            gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorBasculas" ) ), GTK_WIDGET( gtk_builder_get_object( builder, "ItemBascula" ) ), bascula -> obtenerCodigo() );
            gtk_combo_box_text_append( GTK_COMBO_BOX_TEXT( buscar_objeto( "BasculasRegistradas" ) ), NULL, bascula -> obtenerNombre().c_str() );
        }

        // Si no hay basculas registradas muestra un mensaje de vacío
        if( basculasRegistradas.size() < 1 ){
            gtk_widget_show( GTK_WIDGET( buscar_objeto( "MensajeBasculas" ) ) );
            gtk_widget_hide( GTK_WIDGET( buscar_objeto( "ContenedorBasculas" ) ) );
        }
        else{
            gtk_widget_show( GTK_WIDGET( buscar_objeto( "ContenedorBasculas" ) ) );
            gtk_widget_hide( GTK_WIDGET( buscar_objeto( "MensajeBasculas" ) ) );
        }
    }
    catch( runtime_error &re ){
        throw re;
    }
}

void basculaRegistrarPeso( string etiquetaPeso, string etiquetaHora )
{
    try{
        // Cierra el lector
        basculaCerrarLector();

        // Si hay un hilo activo lo cierra
        if( lectura != nullptr ){
            lectura -> join();
        }
        
        // Establece la opción por defecto
        gtk_combo_box_set_active( GTK_COMBO_BOX( buscar_objeto( "BasculasRegistradas" ) ), 0 );

        // Lee las etiquetas de peso
        string peso = ( bascula != nullptr ? pesoString( bascula -> leer(), 2, true ) : "0.00 Kg" );
        gtk_label_set_text( GTK_LABEL( buscar_objeto( etiquetaPeso ) ), peso.c_str() );
        gtk_label_set_text( GTK_LABEL( buscar_objeto( etiquetaHora ) ), tiempo_leer_hora( 1 ).c_str() );
    }
    catch( exception e ){
        cout << e.what() << endl;
    }
}

// Busca el usuario por su nombre de usuario (valga la redundancia)
Bascula *basculaBuscarPorCodigo( unsigned int codigo )
{
    for( list< Bascula * >::iterator registro = basculasRegistradas.begin(); registro != basculasRegistradas.end(); registro++ ){
        if( codigo == (*registro) -> obtenerCodigo() ){
            return (*registro);
        }
    }
    
    return nullptr;
}
