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
    interfaz.mostrarElemento( "VentanaLectorPeso" );
    interfaz.establecerTextoEtiqueta( "EstadoLectura", "Esperando selección" );

    // Establece el peso en 0 Kg
    interfaz.establecerTextoEtiqueta( "EtiquetaPeso", "0 Kg" );

     // Conecta las señales convenientes
    interfaz.conectarSenal( comboBoxBasculasRegistradas, G_CALLBACK( basculaLecturaIniciar ), nullptr );
}

gboolean basculaCerrarLector()
{
    // Oculta la ventana
	interfaz.ocultarElemento( "VentanaLectorPeso" );

    // Desconecta la señal de seleccion de bascula
    interfaz.desconectarSenal( comboBoxBasculasRegistradas );
    
    // Desactiva la báscula seleccionada
    if( bascula != nullptr ){
        bascula -> establecerActivo( false );
    }

    return TRUE;
}

void basculaLecturaIniciar()
{
    bascula = basculaBuscarPorCodigo( gtk_combo_box_get_active( GTK_COMBO_BOX( interfaz.obtenerObjeto( "BasculasRegistradas" ) ) ) );
    if( bascula != nullptr ){
        lectura = new thread( basculaLecturaActualizar );
    }
}

void basculaLecturaActualizar()
{   
    // Establece el estado del lector
    interfaz.establecerTextoEtiqueta( "EstadoLectura", "Conectando a " + bascula -> obtenerPuerto() );

    // Conecta con la báscula
    bascula -> conectar();
    
    // Indica que está leyendo
    interfaz.establecerTextoEtiqueta( "EstadoLectura", "Leyendo" );
    
    // Comienza a actualizar los datos de la báscula
    while( bascula -> estaActivo() ){
        // Actualiza el peso actual leído
        bascula -> actualizarPeso();

        // Lee el peso del lector
        double peso = bascula -> leer();

		// Establece el peso en la etiqueta
		interfaz.establecerTextoEtiqueta( "EtiquetaPeso", pesoString( peso, 2, true ) );
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
        consulta = "select max( clave ) as clave from basculas";
        database.open( databaseFile );
        database.query( consulta );
        database.close();

        codigoBasculaActual = ( results.size() > 0 ? stoi( (* results.at( 0 ))[ "clave" ] ) : 0 );

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
	interfaz.limpiarComboBoxText( "OpcionesBasculaPuerto" );
	
	// Agrega las opciones seleccionar y las establece como por defecto
	interfaz.agregarOpcionComboBoxText( "OpcionesBasculaPuerto", "Seleccionar", "Seleccionar" );
	interfaz.establecerActivoComboBoxText( "OpcionesBasculaPuerto", "Seleccionar" );

	// Agrega los puertos seriales disponibles
	char directorioPuerto[ 5000 ];

	// Revisa si hay puertos disponibles
	int contadorDispositivos = 0;
	for( unsigned contador = 0; contador < 255; ++contador ){
		string nombrePuerto = "COM" + to_string( contador );
		DWORD intento = QueryDosDevice( nombrePuerto.c_str(), directorioPuerto, 5000 );
		if( intento != 0 ){
			interfaz.agregarOpcionComboBoxText( "OpcionesBasculaPuerto", ( contador > 9 ? "\\\\.\\" + nombrePuerto : nombrePuerto ).c_str(), ( contador > 9 ? "\\\\.\\" + nombrePuerto : nombrePuerto ).c_str() );
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
        Bascula *bascula = basculaBuscarPorCodigo( stoi( interfaz.obtenerTextoEtiqueta( "OpcionesBasculaCodigo" ) ) );
        if( bascula == nullptr ){
            // Es un nuevo registro
            bascula = new Bascula();

            // Establece el código
            bascula -> establecerCodigo( ++codigoBasculaActual );
            nuevaBascula = true;
        }

        // Establece los datos de configuración establecidos
        bascula -> establecerNombre( interfaz.obtenerTextoEntrada( "OpcionesBasculaNombre" ) );
        bascula -> establecerPuerto( interfaz.obtenerOpcionComboBoxText( "OpcionesBasculaPuerto" ) );
        bascula -> establecerBaudRate( interfaz.obtenerOpcionComboBoxText( "OpcionesBasculaBaudrate" ) );
        bascula -> establecerByteSize( interfaz.obtenerTextoEntrada( "OpcionesBasculaBitsDatos" ) );
        bascula -> establecerStopBits( interfaz.obtenerTextoEntrada( "OpcionesBasculaBitsStop" ) );
        bascula -> establecerParity( interfaz.obtenerOpcionComboBoxText( "OpcionesBasculaParidad" ) );
        bascula -> establecerBytesIgnorados( interfaz.obtenerTextoEntrada( "OpcionesBasculaBytesIgnorados" ) );

        // ¿Es nueva bascula?
        if( nuevaBascula ){
            basculaNuevo( bascula );
        }

        // Actualiza la lista de básculas registradas
        basculaLimpiarFormulario();
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
        Bascula *bascula = basculaBuscarPorCodigo( stoi( interfaz.obtenerWidgetSeleccionadoListBox( "ContenedorBasculas" ) ) );
        if( bascula == nullptr ){
            throw runtime_error( "Ocurrió un error al recuperar un ticket seleccionado." );
        }

        // Agrega la información a la base de datos si llegó aquí
        stringstream consulta;
        consulta << "delete from basculas where clave = " << bascula -> obtenerCodigo() ; 

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
        interfaz.removerElementosHijos( "ContenedorBasculas" );
        interfaz.limpiarComboBoxText( "BasculasRegistradas" );
        interfaz.agregarOpcionComboBoxText( "BasculasRegistradas", "Seleccionar", NULL );
        interfaz.establecerOpcionComboBox( "BasculasRegistradas", 0 );

        // Para cada una de las básculas registradas
        for( Bascula *bascula : basculasRegistradas ){
            // Crea un elemento que será añadido a la interfaz
            Widget *elemento = new Widget();

            // Carga el elemento y lo ancla hacia las opciones
            elemento -> cargarWidget( "../recursos/interfaces/ElementoBascula.glade" );
            elemento -> establecerNombreWidget( "ItemBascula", to_string( bascula -> obtenerCodigo() ) );
            elemento -> establecerTextoEtiqueta( "ItemBasculaNombre", bascula -> obtenerNombre() );
            elemento -> establecerTextoEtiqueta( "ItemBasculaPuerto", bascula-> obtenerPuerto() );
            elemento -> establecerTextoEtiqueta( "ItemBasculaVelocidad", to_string( bascula -> obtenerBaudRate() ) + " bps"  );

            // Lo agrega al contenedor y la lista de basculas registradas
            interfaz.insertarElementoListBox( elemento, "ItemBascula", "ContenedorBasculas", bascula -> obtenerCodigo() );
            interfaz.agregarOpcionComboBoxText( "BasculasRegistradas", bascula -> obtenerNombre(), NULL );

            // Elimina el widget creado
            delete elemento;
        }

        // Si no hay basculas registradas muestra un mensaje de vacío
        if( basculasRegistradas.size() < 1 ){
            interfaz.mostrarElemento( "MensajeBasculas" );
            interfaz.ocultarElemento( "ContenedorBasculas" );
        }
        else{
            interfaz.mostrarElemento( "ContenedorBasculas" );
            interfaz.ocultarElemento( "MensajeBasculas" );
        }
    }
    catch( runtime_error &re ){
        throw re;
    }
}

void basculaLimpiarFormulario( void )
{
    interfaz.establecerTextoEtiqueta( "OpcionesBasculaCodigo", to_string( codigoBasculaActual + 1 ) );
    interfaz.establecerTextoEntrada( "OpcionesBasculaNombre", "" );
    interfaz.establecerOpcionComboBox( "OpcionesBasculaPuerto", 0 );
    interfaz.establecerOpcionComboBox( "OpcionesBasculaBaudrate", 6 );
    interfaz.establecerTextoEntrada( "OpcionesBasculaBitsDatos", "8" );
    interfaz.establecerTextoEntrada( "OpcionesBasculaBitsStop", "0" );
    interfaz.establecerOpcionComboBox( "OpcionesBasculaParidad", 0 );
    interfaz.establecerTextoEntrada( "OpcionesBasculaBytesIgnorados", "0" );
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
        interfaz.establecerOpcionComboBox( "BasculasRegistradas", 0 );

        // Lee las etiquetas de peso
        if( bascula != nullptr ){
            string peso = pesoString( bascula -> leer(), 2, true );
            interfaz.establecerTextoEtiqueta( etiquetaPeso, peso );
            interfaz.establecerTextoEtiqueta( etiquetaHora, tiempo_leer_hora( 1 ) );
        }
        else{
            interfaz.establecerTextoEtiqueta( etiquetaPeso, "0.00 kg" );
            interfaz.establecerTextoEtiqueta( etiquetaHora, tiempo_leer_hora( 1 ) );
        }
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
