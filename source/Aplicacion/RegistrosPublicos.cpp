#include "RegistrosPublicos.h"
#include "Aplicacion.h"
#include "GestorBasculas.h"
#include "Funciones.h"
#include "Vistas.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
using namespace std;

TicketPublico *registroPublico = nullptr;
bool publicoPendiente = true;

void publicoRegistrarPendiente()
{
	// Bandera que indica si se va a actualizar o registrar el nuevo ticket
	bool esRegistroNuevo = false;

	// Crea el nuevo registro
	TicketPublico *registroPublico;

	try{
		// Revisa si el ticket con el folio indicado existe como pendiente, si no, se crea uno nuevo
		registroPublico = buscarRegistroPublicoPorFolio( stoi( interfaz.obtenerTextoEtiqueta( "EntradaFolioPublico" ) ), registrosPublicosPendientes );
		if( registroPublico == nullptr ){
			++folioActualPublico;
			esRegistroNuevo = true;
			registroPublico = new TicketPublico();
			registroPublico -> establecerFolio( stoi( interfaz.obtenerTextoEtiqueta( "EntradaFolioPublico" ) ) );
		}

		// Obtiene el producto introducido
   		string nombreProducto = interfaz.obtenerTextoEntrada( "EntradaNombreProductoPublico" );
    	Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );

    	// Establece los datos obligatorios
		registroPublico -> establecerFecha( interfaz.obtenerTextoEtiqueta( "EntradaFechaPublico" ) );
		registroPublico -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		registroPublico -> establecerNombreConductor( interfaz.obtenerTextoEntrada( "EntradaNombreConductorPublico" ) );
		registroPublico -> establecerNumeroPlacas( interfaz.obtenerTextoEntrada( "EntradaNumeroPlacasPublico" ) );
		registroPublico -> establecerEntradaManual( 0 );// lectorBascula.lecturaManualActivada() );
		registroPublico -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );
		registroPublico -> establecerPendiente( true );

		// Registra el tipo de registro
		if( interfaz.obtenerEstadoBotonToggle( "ViajeLocal" ) ){
	    	registroPublico -> establecerTipoViaje( VIAJE_LOCAL );
		}
		else{
	    	registroPublico -> establecerTipoViaje( VIAJE_FORANEO );
		}

		// Registra el peso bruto
		try{
			registroPublico -> establecerPesoBruto( interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoPublico" ) );
			registroPublico -> establecerHoraEntrada( interfaz.obtenerTextoEtiqueta( "EntradaHoraEntradaPublico" ) );
			registroPublico -> establecerPesoBrutoEstablecido( true );
		}
		catch( invalid_argument &ia ){
			registroPublico -> establecerPesoBruto( 0.f );
			registroPublico -> establecerHoraEntrada( "" );
			registroPublico -> establecerPesoBrutoEstablecido( false );
		}

		// Registra el peso tara
		try{
			registroPublico -> establecerPesoTara( interfaz.obtenerTextoEtiqueta( "EntradaPesoTaraPublico" ) );
			registroPublico -> establecerHoraSalida( interfaz.obtenerTextoEtiqueta( "EntradaHoraSalidaPublico" ) );
			registroPublico -> establecerPesoTaraEstablecido( true );
		}
		catch( invalid_argument &ia ){
			registroPublico -> establecerPesoTara( 0.f );
			registroPublico -> establecerHoraSalida( "" );
			registroPublico -> establecerPesoTaraEstablecido( false );
		}

		// Registra el peso neto
		try{
			// ¿Están establecidos el peso bruto y el peso tara?
			if( registroPublico -> estaPesoBrutoEstablecido() && registroPublico -> estaPesoTaraEstablecido() ){
				registroPublico -> establecerPesoNeto( interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoPublico" ) );
				registroPublico -> establecerPesoNetoEstablecido( true );
			}
			else{
				// No registra un peso neto
				registroPublico -> establecerPesoNeto( 0.f );
				registroPublico -> establecerPesoNetoEstablecido( false );
			}
		}
		catch( invalid_argument &ia ){
			// No registra un peso neto
			registroPublico -> establecerPesoNeto( 0.f );
			registroPublico -> establecerPesoNetoEstablecido( false );
		}

		// Crea o registra el registro según corresponda
		if( esRegistroNuevo ){
			crearRegistroPublicoPendiente( registroPublico );
			mostrarMensajeError( "Registro creado correctamente." );
		}
		else{
			actualizarRegistroPublicoPendiente( registroPublico );
			mostrarMensajeError( "Registro actualizado correctamente." );
		}

		// Obtiene los tickets pendientes
		publicoActualizarRegistros( registrosPublicosPendientes, "ContenedorTickets" );

		// Establece las vistas
		regresarVista();
	}
	catch( invalid_argument &ia ){
		folioActualPublico--;
		mostrarMensajeError( ia.what() );
		if( esRegistroNuevo ){
			delete registroPublico;
			registroPublico = nullptr;
		}
	}
}

// Finalizar ticket
void publicoFinalizarPendiente()
{
	// Bandera que indica si se va a actualizar o registrar el nuevo ticket
	bool esRegistroNuevo = false;

	// Crea el nuevo registro
	TicketPublico *registroPublico;

	try{
		// Revisa si el ticket con el folio indicado existe como pendiente, si no, se crea uno nuevo
		registroPublico = buscarRegistroPublicoPorFolio( stoi( interfaz.obtenerTextoEtiqueta( "EntradaFolioPublico" ) ), registrosPublicosPendientes );
		if( registroPublico == nullptr ){
			++folioActualPublico;
			esRegistroNuevo = true;
			registroPublico = new TicketPublico();
			registroPublico -> establecerFolio( stoi( interfaz.obtenerTextoEtiqueta( "EntradaFolioPublico" ) ) );
		}

		// Obtiene el producto introducido
   		string nombreProducto = interfaz.obtenerTextoEntrada( "EntradaNombreProductoPublico" );
    	Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );

    	// Establece los datos obligatorios
		registroPublico -> establecerFecha( interfaz.obtenerTextoEtiqueta( "EntradaFechaPublico" ) );
		registroPublico -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		registroPublico -> establecerNombreConductor( interfaz.obtenerTextoEntrada( "EntradaNombreConductorPublico" ) );
		registroPublico -> establecerNumeroPlacas( interfaz.obtenerTextoEntrada( "EntradaNumeroPlacasPublico" ) );
		registroPublico -> establecerEntradaManual( 0 );//lectorBascula.lecturaManualActivada() );
		registroPublico -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );

		// Registra el tipo de registro
		if( interfaz.obtenerEstadoBotonToggle( "ViajeLocal" ) ){
	    	registroPublico -> establecerTipoViaje( VIAJE_LOCAL );
		}
		else{
	    	registroPublico -> establecerTipoViaje( VIAJE_FORANEO );
		}

		// Registra el peso bruto
		registroPublico -> establecerPesoBruto( interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoPublico" ) );
		registroPublico -> establecerHoraEntrada( interfaz.obtenerTextoEtiqueta( "EntradaHoraEntradaPublico" ) );
		registroPublico -> establecerPesoBrutoEstablecido( true );

		// Registra el peso tara
		registroPublico -> establecerPesoTara( interfaz.obtenerTextoEtiqueta( "EntradaPesoTaraPublico" ) );
		registroPublico -> establecerHoraSalida( interfaz.obtenerTextoEtiqueta( "EntradaHoraSalidaPublico" ) );
		registroPublico -> establecerPesoTaraEstablecido( true );

		// Registra el peso neto
		// ¿Están establecidos el peso bruto y el peso tara?
		if( registroPublico -> estaPesoBrutoEstablecido() && registroPublico -> estaPesoTaraEstablecido() ){
			registroPublico -> establecerPesoNeto( interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoPublico" ) );
			registroPublico -> establecerPesoNetoEstablecido( true );
		}
		else{
			mostrarMensajeError( "No se han establecido todos los campos necesarios para\nregistrar el peso neto." );
			if( esRegistroNuevo ){
				delete registroPublico;
				registroPublico = nullptr;
				folioActualPublico--;
			}
			return;
		}

		// Registro correcto, ya no es un registro pendiente
		registroPublico -> establecerPendiente( false );

		// Finaliza el registro
		finalizarRegistroPublico( registroPublico, esRegistroNuevo );

		// Actualiza la vista de tickets
		publicoActualizarRegistros( registrosPublicosPendientes, "ContenedorTickets" );
		
		// Indica que se creo correctamente el registro
		mostrarMensajeError( "Registro finalizado. Se creará formato de impresión si se encuentra habilitado." );

		// Establece las vistas
		regresarVista();
	}
	catch( invalid_argument &ia ){
		mostrarMensajeError( ia.what() );
		if( esRegistroNuevo ){
			delete registroPublico;
			registroPublico = nullptr;
			folioActualPublico--;
		}
	}
}

// Registra el peso bruto
void publicoRegistrarPesoBruto()
{
	// Registra el peso bruto
	basculaRegistrarPeso( "EntradaPesoBrutoPublico", "EntradaHoraEntradaPublico" );
	
    // Intenta calcular el peso neto
    publicoActualizarPesoNeto();
}

// Registra el peso tara
void publicoRegistrarPesoTara()
{
	// Registra el peso tara
	basculaRegistrarPeso( "EntradaPesoTaraPublico", "EntradaHoraSalidaPublico" );
    
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
		interfaz.ocultarElemento( "MensajeError" );
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoPublico", pesoString( publicoCalcularPesoNeto( pesoBruto, pesoTara ), 2 ) );
	}
	else{
		// No pasa nada, solo indicamos que no se estableció el peso neto e impedimos que lo registre como finalizado		
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", "No establecido" );
		interfaz.establecerBotonEtiqueta( "BotonRegistrarInterno", "Pendiente" );
	}
}

// Cancela la finalización publico
void publicoCancelarFinalizacion()
{
    registroPublico = nullptr;
	
    regresarVista();
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
	regresarVista();
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

	// Agrega el caracter BOM para archivos UTF-8
	const char *bom = "\xef\xbb\xbf";
    archivo << bom;

	// Consulta 
    string consulta = "select folio, fecha, tipo_viaje, nombre_producto, numero_placas, nombre_conductor, hora_entrada, "
    				  "hora_salida, peso_bruto, peso_tara, peso_neto, entrada_manual, nombre_basculista "
    				  "from registros_publicos join productos on registros_publicos.clave_producto = productos.clave_producto "
    				  "where pendiente = 0 and fecha between '" + obtenerFecha( diaInicio, mesInicio + 1, anioInicio ) + 
    				  "' and '" + obtenerFecha( diaFin, mesFin + 1, anioFin ) + "' " + "order by fecha";
    
	// Realiza la consulta y envía los resultados al archivos
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
	regresarVista();
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
			elemento -> establecerNombreWidget( "Ticket", to_string( (*ticket) -> obtenerFolio() ) );
		    elemento -> establecerTextoEtiqueta( "ItemEntradaFolioInterno", clave.str() );
		    elemento -> establecerTextoEtiqueta( "ItemEntradaFechaInterno", (*ticket) -> obtenerFecha() );
		    elemento -> establecerTextoEtiqueta( "ItemEntradaProductoInterno", (*ticket) -> obtenerProducto() -> obtenerNombre() );
		    elemento -> establecerTextoEtiqueta( "ItemEntradaPlacaInterno", (*ticket) -> obtenerNumeroPlacas() );
		    interfaz.insertarElementoListBox( elemento, "Ticket", idContenedor, (*ticket) -> obtenerFolio() );
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
	// Busca el ticket publico y lo imprime
	TicketPublico *registroPublico = buscarRegistroPublicoPorFolio( stoi( interfaz.obtenerTextoEtiqueta( "FolioPublico" ) ), registrosPublicosConsultados );
	if( registroPublico != nullptr ){
		registroPublico -> imprimir( nombreEmpresa );

		mostrarMensaje( "Formato de impresión creado." );
	}
}

void publicoGenerarInforme()
{
	mostrarMensaje( "Los informes para registros públicos\nno están disponibles." );
}
