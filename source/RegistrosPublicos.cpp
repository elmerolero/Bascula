#include "RegistrosPublicos.h"
#include "Aplicacion.h"
#include "LectorBascula.h"
#include "Funciones.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
using namespace std;

TicketPublico *registroPublico = nullptr;
bool publicoPendiente = true;

TicketPublico *crearRegistroPublico()
{
	// Crea el ticket publico
	TicketPublico *ticket = new TicketPublico();
	if( ticket == nullptr ){
		throw runtime_error( "Hubo un error al crear el ticket." );
	}

	return ticket;
}

void publicoRegistrarPendiente()
{
	// Crea el nuevo registro
	TicketPublico *registroPublico;

	try{
		// Crea el nuevo registro
		registroPublico = crearRegistroPublico();

		// Obtiene el producto introducido
   		string nombreProducto = interfaz.obtenerTextoEntrada( "EntradaNombreProductoPublico" );
    	Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );

    	// Establece los datos obligatorios
    	registroPublico -> establecerFolio( ++folioActualPublico );
		registroPublico -> establecerFecha( interfaz.obtenerTextoEtiqueta( "EntradaFechaPublico" ) );
		registroPublico -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		registroPublico -> establecerNombreConductor( interfaz.obtenerTextoEntrada( "EntradaNombreConductorPublico" ) );
		registroPublico -> establecerNumeroPlacas( interfaz.obtenerTextoEntrada( "EntradaNumeroPlacasPublico" ) );
		registroPublico -> establecerEntradaManual( lectorBascula.lecturaManualActivada() );
		registroPublico -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );

		// Registra el peso bruto
		registroPublico -> establecerPesoBruto( interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoPublico" ) );
		registroPublico -> establecerHoraEntrada( interfaz.obtenerTextoEtiqueta( "EntradaHoraEntradaPublico" ) );
		registroPublico -> establecerPesoBrutoEstablecido( true );

		// Registra el tipo de registro
		if( interfaz.obtenerEstadoBotonToggle( "ViajeLocal" ) ){
	    	registroPublico -> establecerTipoViaje( VIAJE_LOCAL );
		}
		else{
	    	registroPublico -> establecerTipoViaje( VIAJE_FORANEO );
		}

		// Registra si está pendiente
		registroPublico -> establecerPendiente( true );
		
		// Crea el registro publico pendiente
		crearRegistroPublicoPendiente( registroPublico );

		// Muestra mensaje de que el registro fue correctamente registrado
		mostrarMensaje( "Registro creado\ncorrectamente." );
	
		// Actualiza la vista de tickets
		publicoActualizarRegistros( registrosPublicosPendientes, "ContenedorRegistros" );
		
		// Establece las vistas
		irHacia( nullptr, (void *)"Tickets" );
	}
	catch( invalid_argument &ia ){
		delete registroPublico;
		registroPublico = nullptr;
		folioActualPublico--;
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampoPublico", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampoPublico" );
	}
	catch( runtime_error &re ){
		folioActualPublico--;
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampoPublico", re.what() );
		interfaz.mostrarElemento( "MensajeErrorCampoPublico" );
	}
}

// Finalizar ticket
void publicoFinalizarPendiente()
{
	try{
		// Registra el nuevo peso tara
		registroPublico -> establecerPesoTara( interfaz.obtenerTextoEtiqueta( "EntradaPesoTaraPublico" ) );
		registroPublico -> establecerHoraSalida( interfaz.obtenerTextoEtiqueta( "EntradaHoraSalidaPublico" ) );
		registroPublico -> establecerPesoTaraEstablecido( true );

		// Registra el nuevo peso neto
		if( registroPublico -> estaPesoBrutoEstablecido() && registroPublico -> estaPesoTaraEstablecido() && registroPublico -> estaPendiente() ){
			registroPublico -> establecerPesoNeto( interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoPublico" ) );
			registroPublico -> establecerPesoNetoEstablecido( true );
		}
		else{
			interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Es necesario registrar todos los campos adecuadamente para calcular el peso neto." );
			interfaz.mostrarElemento( "MensajeErrorCampo" );
			return;
		}

		// Actualiza el tipo de viaje en caso de que hubiera cambiado
	    if( interfaz.obtenerEstadoBotonToggle( "ViajeLocal" ) ){
			registroPublico -> establecerTipoViaje( VIAJE_LOCAL );
	    }
	    else{
			registroPublico -> establecerTipoViaje( VIAJE_FORANEO );
	    }
	    
	    registroPublico -> establecerPendiente( false );
	    
	    finalizarRegistroPublico( registroPublico );
	    
	    // Actualiza la vista de tickets
	    publicoActualizarRegistros( registrosPublicosPendientes, "ContenedorTickets" );
			
	    // Establece las vistas
	    irHacia( nullptr, (void *)"Tickets" );
		
	    // Muestra un mensaje de que fue finalizado correctamente
	    mostrarMensaje( "Registro realizado correctamente." );

	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampoPublico", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampoPublico" );
	}
}

// Registra el peso bruto
void publicoRegistrarPesoBruto()
{
    // Cierra la báscula
   	lectorBascula.cerrar();
		
	// Indica en la interfaz los datos registrados
	try{
		interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoPublico", lectorBascula.leer() );
		interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaPublico", obtenerHora() );
    }
    catch( invalid_argument &ia ){
    	interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoPublico", "No establecido" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaPublico", "No establecida" );
    }
    
    // Intenta calcular el peso neto
    publicoActualizarPesoNeto();
}

// Registra el peso tara
void publicoRegistrarPesoTara()
{
	// Cierra el lector de la báscula
    lectorBascula.cerrar();
    
    try{
		// Establece el peso leído de la etiqueta
		interfaz.establecerTextoEtiqueta( "EntradaPesoTaraPublico", lectorBascula.leer() );
		interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaPublico", obtenerHora() );
		
		// Habilita la opción de poder finalizar ticket
		interfaz.establecerBotonEtiqueta( "BotonRegistrarPublico", "Finalizar" );
    }
    catch( invalid_argument &ia ){
    	// Muestra el error sucedido
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampoPublico", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampoPublico" );
		
		// Reestablece el campo de la hora y el peso de entrada
		interfaz.establecerTextoEtiqueta( "EntradaPesoTaraPublico", "No establecido" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaPublico", "No establecida" );
    }
    
    // Calcula el peso neto
    publicoActualizarPesoNeto();
}

//
double publicoCalcularPesoNeto( double pesoBruto, double pesoTara )
{
	return abs( pesoBruto - pesoTara );
}

// Actualiza el peso neto
void publicoActualizarPesoNeto()
{
	// Variables
	double pesoBruto;
	double pesoTara;
	bool completo = true;

	// Intenta obtener el peso bruto leído
	try{
		pesoBruto = stod( interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoPublico" ) );
	}
	catch( invalid_argument &ia ){
		completo = false;
	}

	// Intenta obtener el peso tara
	try{
		pesoTara = stod( interfaz.obtenerTextoEtiqueta( "EntradaPesoTaraPublico" ) );
	}
	catch( invalid_argument &ia ){
		completo = false;
	}

	// Intenta calculara el peso neto
	if( completo ){
		interfaz.ocultarElemento( "MensajeErrorCampoPublico" );
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoPublico", pesoString( publicoCalcularPesoNeto( pesoBruto, pesoTara ), 2 ) );
	}
	else{
		// No pasa nada, solo indicamos que no se estableció el peso neto e impedimos que lo registre como finalizado		
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", "No establecido" );
		interfaz.establecerBotonEtiqueta( "BotonRegistrarInterno", "Pendiente" );
	}
}

// Selecciona el tipo de registro publico
void publicoSeleccionarTipo()
{
     if( registroPublico != nullptr ){
	if( interfaz.obtenerEstadoBotonToggle( "ViajeLocal" ) ){
	     registroPublico -> establecerTipoViaje( VIAJE_LOCAL );
	}
	else{
	     registroPublico -> establecerTipoViaje( VIAJE_FORANEO );
	}
     }
}

// Cancela el registro de un ticket publico
void publicoCancelarRegistro()
{
     if( registroPublico != nullptr ){
	delete registroPublico;
	registroPublico = nullptr;
     }
	
     irHacia( nullptr, (void *)"Tickets" );
}

// Cancela la finalización publico
void publicoCancelarFinalizacion()
{
    registroPublico = nullptr;
	
    irHacia( nullptr, (void *)"Tickets" );
}

// Obtiene los tickets del día seleccionado
void publicoSeleccionarDia()
{
	// Variables de la fecha del dia seleccionado
	unsigned int dia, mes, anio;

	// Obtiene la fecha
	interfaz.obtenerFechaCalendario( "EntradaDiaConsultar", &dia, &mes, &anio );

	// Obtiene los tickets de la fecha seleccionada
	publicoObtenerPorFecha( registrosPublicosConsultados, obtenerFecha( dia, mes + 1, anio ) );

	// Establece la fecha del ticket que se está consultando
	interfaz.establecerTextoEtiqueta( "TicketsRegistrados", "Registros del día " + obtenerFecha( dia, mes, anio ) + ":"  );

	// Establece el número de tickets
	interfaz.establecerTextoEtiqueta( "TicketsContados", to_string( registrosPublicosConsultados.size() ) + " registros"  );

	// Actualiza la lista de tickets
	publicoActualizarRegistros( registrosPublicosConsultados, "ContenedorRegistrosPesaje" );

	// Regresa a la vista anterior
	irHacia( nullptr, (void *)"Pesajes" );
}

void publicoObtenerRegistrosRango()
{
	// Muestra un mensaje que indica que se está generando el informe
	mostrarMensaje( "Obteniendo registros.\nPor favor espere.");

	// Variables para el rango de seleccion de fechas
	unsigned int diaInicio, mesInicio, anioInicio;
	unsigned int diaFin, mesFin, anioFin;

	// Obtiene las fechas
	interfaz.obtenerFechaCalendario( "FechaInformeInicio", &diaInicio, &mesInicio, &anioInicio );
	interfaz.obtenerFechaCalendario( "FechaInformeFin", &diaFin, &mesFin, &anioFin );

	// Archivo con el informe
    ofstream archivo;

	// Se abre el archivo
    archivo.open( "../resources/data/Registros.csv", ios_base::out );
    if( !archivo ){
        mostrarMensaje( "No se pudo abrir el archivo.\n"
        				"Asegúrate de que ningún programa esté haciendo uso de el archivo,\n"
        				"de tener espacio en el disco duro e inténtalo de nuevo." );
        return;
    }

    string consulta = "select folio, fecha, tipo_viaje, nombre_producto, numero_placas, nombre_conductor, hora_entrada, "
    				  "hora_salida, peso_bruto, peso_tara, peso_neto, entrada_manual, nombre_basculista "
    				  "from registros_publicos join productos on registros_publicos.clave_producto = productos.clave_producto "
    				  "where pendiente = 0 and fecha between '" + obtenerFecha( diaInicio, mesInicio + 1, anioInicio ) + 
    				  "' and '" + obtenerFecha( diaFin, mesFin + 1, anioFin ) + "' " + "order by fecha";	
    database.open( nombreArchivo );
    database.query( consulta );
    if( rows.size() > 0 ){
    	archivo << "Folio, Fecha, Viaje, Producto, No. Placas, Conductor, Hora entrada, Hora salida, Peso bruto, Peso tara, Peso neto, Manual, Basculista" << endl;
    	for( Row *row : rows ){
			archivo << row -> columns.at( 0 ) << ", " << row -> columns.at( 1 ) << ", " << ( stoi( row -> columns.at( 2 ) ) == VIAJE_LOCAL ? "Local" : "Foráneo" ) << ", " 
					<< row -> columns.at( 3 ) << ", " << row -> columns.at( 4 ) << ", " << row -> columns.at( 5 ) << ", " << row -> columns.at( 6 ) 
					<< ", " << row -> columns.at( 7 ) << ", " << row -> columns.at( 8 ) << ", " << row -> columns.at( 9 ) << ", " << row -> columns.at( 10 ) 
					<< ", " << row -> columns.at( 11 ) << ", " << row -> columns.at( 12 ) << ", " << endl;
    	}
    }
    database.close();
    archivo.close();

    ShellExecute(NULL, "open", "../resources/data/Registros.csv", NULL, NULL, SW_HIDE );
	irHacia( nullptr, (void *)"Pesajes" );
}

// Obtiene los registros de la fecha seleccionada
void publicoObtenerPorFecha( list< TicketPublico * > &registros, std::string fecha )
{
	// Limpia la lista de los registros a consultar
	limpiarRegistrosPublicos( registrosPublicosConsultados );
	
	// Conecta con la base de datos
	database.open( nombreArchivo );

	// Genera el comando de consulta para obtener los tickets
	string consulta = "select * from registros_publicos where pendiente = 0 and fecha = '" + fecha + "'";

	// Realiza la consulta
	database.query( consulta );

	// ¿Hay resultados?
	if( rows.size() > 0 ){
		for( Row *row : rows ){
			// Crea el nuevo ticket
			TicketPublico *ticket = new TicketPublico();

			try{
    		    // Establece los datos del registro
    		    establecerRegistroPublicoDesdeRenglon( ticket, row );
    				
    		    // Lo agrega al campo de registros internos pendientes
    		    registros.push_back( ticket );
    	    }
    	    catch( invalid_argument &ia ){
    			cerr << ia.what() << endl;
    	    }
		}
	}
	else{
		mostrarMensaje( "No se encontraron registros del día seleccionado." );
	}

	// Cierra la base de datos
	database.close();
}

// Actualiza los registros publicos pendientes
void publicoActualizarRegistros( list< TicketPublico * > &ticketsPublicos, std::string idContenedor )
{
    // Limpia el contenedor
    interfaz.removerElementosHijos( idContenedor );

    // Itera a través de la lista de tickets pendientes y crea los tickets necesarios
    for( list< TicketPublico * >::iterator ticket = ticketsPublicos.begin(); ticket != ticketsPublicos.end(); ticket++ ){
		// Crea un elemento que será añadido a la interfaz
		Widget *elemento = new Widget();

		stringstream clave;
		clave << setfill( '0' ) << setw( 7 ) << (*ticket) -> obtenerFolio();

		try{
		    elemento -> cargarWidget( "../resources/interfaces/ElementoTicketPublico.glade" );
		    elemento -> establecerTextoEtiqueta( "ItemEntradaFolioInterno", clave.str() );
		    elemento -> establecerTextoEtiqueta( "ItemEntradaFechaInterno", (*ticket) -> obtenerFecha() );
		    elemento -> establecerTextoEtiqueta( "ItemEntradaProductoInterno", (*ticket) -> obtenerProducto() -> obtenerNombre() );
		    elemento -> establecerTextoEtiqueta( "ItemEntradaPlacaInterno", (*ticket) -> obtenerNumeroPlacas() );
		    interfaz.insertarElementoAGrid( elemento, "Ticket", idContenedor, 0, (*ticket) -> obtenerFolio(), 1, 1 );
	    }
	    catch( runtime_error &re ){
		    cerr << re.what() << endl;
	    }
		
		delete elemento;
    }
}

// Eliminar el registro seleccionado
void publicoEliminarSeleccionado()
{
	// Elimina el registro de la base de datos
	database.open( nombreArchivo );

	// Comando de consulta
	string consulta = "delete from registros_publicos where folio = " + to_string( registroPublico -> obtenerFolio() );
	database.query( consulta );

	database.close();

	// Elimina el registro de la lista
	if( registroPublico != nullptr ){
		registrosPublicosConsultados.remove( registroPublico );
		delete registroPublico;
		registroPublico = nullptr;
	}
	else{
		cerr << "Alerta, intento de eliminación de un registro nulo." << endl;
	}

	// Muestra un mensaje que indica que el registro fue eliminado correctamente
	mostrarMensaje( "Registro eliminado" );

	// Cierra la ventana
	interfaz.ocultarElemento( "VentanaSiNo" );

	// Redirige hacia la vista de consulta de registros
	publicoActualizarRegistros( registrosPublicosConsultados, "ContenedorRegistrosPesaje" );
	irHacia( nullptr, (void *)"Pesajes" );
}

// Manda a imprimir el registro interno seleccionado
void publicoImprimirSeleccionado()
{
	if( registroPublico != nullptr ){
		registroPublico -> imprimir( nombreEmpresa );

		mostrarMensaje( "Formato de impresión creado." );
	}
}

void publicoGenerarInforme()
{
	mostrarMensaje( "Los informes para registros públicos\nno están disponibles." );
}
