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
		registroPublico = buscarRegistroPublicoPorFolio( stoi( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ) ) ), registrosPublicosPendientes );
		if( registroPublico == nullptr ){
			++folioActualPublico;
			esRegistroNuevo = true;
			registroPublico = new TicketPublico();
			registroPublico -> establecerFolio( stoi( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ) ) ) );
		}

		// Obtiene el producto introducido
   		string nombreProducto = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico" ) ) );
    	Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );

    	// Establece los datos obligatorios
		registroPublico -> establecerFecha( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFechaPublico" ) ) ) );
		registroPublico -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		registroPublico -> establecerNombreConductor( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorPublico" ) ) ) );
		registroPublico -> establecerNumeroPlacas( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasPublico" ) ) ) );
		registroPublico -> establecerEntradaManual( 0 );// lectorBascula.lecturaManualActivada() );
		registroPublico -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );
		registroPublico -> establecerPendiente( true );

		// Registra el tipo de registro
		
		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "ViajeLocal" ) ) ) ){
	    	registroPublico -> establecerTipoViaje( VIAJE_LOCAL );
		}
		else{
	    	registroPublico -> establecerTipoViaje( VIAJE_FORANEO );
		}

		// Registra el peso bruto
		try{
			registroPublico -> establecerPesoBruto( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoPublico" ) ) ) );
			registroPublico -> establecerHoraEntrada( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaPublico" ) ) ) );
			registroPublico -> establecerPesoBrutoEstablecido( true );
		}
		catch( invalid_argument &ia ){
			registroPublico -> establecerPesoBruto( 0.f );
			registroPublico -> establecerHoraEntrada( "" );
			registroPublico -> establecerPesoBrutoEstablecido( false );
		}

		// Registra el peso tara
		try{
			registroPublico -> establecerPesoTara( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ) ) );
			registroPublico -> establecerHoraSalida( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaPublico" ) ) ) );
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
				registroPublico -> establecerPesoNeto( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoPublico" ) ) ) );
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
			app_mostrar_error( "Registro creado correctamente." );
		}
		else{
			actualizarRegistroPublicoPendiente( registroPublico );
			app_mostrar_error( "Registro actualizado correctamente." );
		}

		// Obtiene los tickets pendientes
		publicoActualizarRegistros( registrosPublicosPendientes, "ContenedorTickets" );

		// Establece las vistas
		regresarVista();
	}
	catch( invalid_argument &ia ){
		folioActualPublico--;
		app_mostrar_error( ia.what() );
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
		registroPublico = buscarRegistroPublicoPorFolio( stoi( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ) ) ), registrosPublicosPendientes );
		if( registroPublico == nullptr ){
			++folioActualPublico;
			esRegistroNuevo = true;
			registroPublico = new TicketPublico();
			registroPublico -> establecerFolio( stoi( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ) ) ) );
		}

		// Obtiene el producto introducido
   		string nombreProducto = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico" ) ) );
    	Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );

    	// Establece los datos obligatorios
		registroPublico -> establecerFecha( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFechaPublico" ) ) ) );
		registroPublico -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		registroPublico -> establecerNombreConductor( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductoPublico" ) ) ) );
		registroPublico -> establecerNumeroPlacas( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasPublico" ) ) ) );
		registroPublico -> establecerEntradaManual( 0 );//lectorBascula.lecturaManualActivada() );
		registroPublico -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );

		// Registra el tipo de registro
		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "ViajeLocal" ) ) ) ){
	    	registroPublico -> establecerTipoViaje( VIAJE_LOCAL );
		}
		else{
	    	registroPublico -> establecerTipoViaje( VIAJE_FORANEO );
		}

		// Registra el peso bruto
		registroPublico -> establecerPesoBruto( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoPublico" ) ) ) );
		registroPublico -> establecerHoraEntrada( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaPublico" ) ) ) );
		registroPublico -> establecerPesoBrutoEstablecido( true );

		// Registra el peso tara
		registroPublico -> establecerPesoTara( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ) ) );
		registroPublico -> establecerHoraSalida( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaPublico" ) ) ) );
		registroPublico -> establecerPesoTaraEstablecido( true );

		// Registra el peso neto
		// ¿Están establecidos el peso bruto y el peso tara?
		if( registroPublico -> estaPesoBrutoEstablecido() && registroPublico -> estaPesoTaraEstablecido() ){
			registroPublico -> establecerPesoNeto( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoPublico" ) ) ) );
			registroPublico -> establecerPesoNetoEstablecido( true );
		}
		else{
			app_mostrar_error( "No se han establecido todos los campos necesarios para\nregistrar el peso neto." );
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
		app_mostrar_error( "Registro finalizado. Se creará formato de impresión si se encuentra habilitado." );

		// Establece las vistas
		regresarVista();
	}
	catch( invalid_argument &ia ){
		app_mostrar_error( ia.what() );
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
		pesoBruto = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoPublico" ) ) ) );
	}
	catch( invalid_argument &ia ){
		completo = false;
	}

	// Intenta obtener el peso tara
	try{
		pesoTara = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ) ) );
	}
	catch( invalid_argument &ia ){
		completo = false;
	}

	// Intenta calculara el peso neto
	if( completo ){
		gtk_widget_hide( GTK_WIDGET( buscar_objeto( "MensajeError" ) ) );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoPublico" ) ), pesoString( publicoCalcularPesoNeto( pesoBruto, pesoTara ), 2 ).c_str() );
	}
	else{
		// No pasa nada, solo indicamos que no se estableció el peso neto e impedimos que lo registre como finalizado
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno" ) ), "No establecido" );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "BotonRegistrarInterno" ) ), "Pendiente" );
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
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "EntradaDiaConsultar" ) ), &dia, &mes, &anio );

	// Obtiene los tickets de la fecha seleccionada
	publicoObtenerPorFecha( registrosPublicosConsultados, tiempo_construir_fecha( dia, mes + 1, anio ) );

	// Establece la fecha del ticket que se está consultando
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "TicketsRegistrados" ) ), ( "Registros del día " + tiempo_construir_fecha( dia, mes, anio ) + ":" ).c_str() );

	// Establece el número de tickets
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "TicketsContados" ) ), ( to_string( registrosPublicosConsultados.size() ) + " registros" ).c_str() );

	// Actualiza la lista de tickets
	publicoActualizarRegistros( registrosPublicosConsultados, "ContenedorRegistrosPesaje" );

	// Regresa a la vista anterior
	regresarVista();
}

void publicoObtenerRegistrosRango()
{
	// Muestra un mensaje que indica que se está generando el informe
	app_mostrar_mensaje( "Obteniendo registros.\nPor favor espere.");

	// Variables para el rango de seleccion de fechas
	unsigned int diaInicio, mesInicio, anioInicio;
	unsigned int diaFin, mesFin, anioFin;

	// Obtiene las fechas
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "FechaInformeInicio" ) ), &diaInicio, &mesInicio, &anioInicio  );
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "FechaInformeFin" ) ), &diaFin, &mesFin, &anioFin );

	// Consulta a realizar
    string consulta = "select folio, fecha, tipo_viaje, nombre_producto, numero_placas, nombre_conductor, hora_entrada, "
    				  "hora_salida, peso_bruto, peso_tara, peso_neto, entrada_manual, nombre_basculista "
    				  "from registros_publicos join productos on registros_publicos.clave_producto = productos.clave_producto "
    				  "where pendiente = 0 and fecha between '" + tiempo_construir_fecha( diaInicio, mesInicio + 1, anioInicio ) + 
    				  "' and '" + tiempo_construir_fecha( diaFin, mesFin + 1, anioFin ) + "' " + "order by fecha";
    
	// Realiza la consulta y envía los resultados al archivos
	database.open( databaseFile );
	database.query( consulta, "../resources/data/Registros.csv" );
    database.close();

    ShellExecute(NULL, "open", "../resources/data/Registros.csv", NULL, NULL, SW_HIDE );
	regresarVista();
}

// Obtiene los registros de la fecha seleccionada
void publicoObtenerPorFecha( list< TicketPublico * > &registros, std::string fecha )
{
	// Limpia la lista de los registros a consultar
	limpiarRegistrosPublicos( registrosPublicosConsultados );
	
	// Conecta con la base de datos
	database.open( databaseFile );

	// Genera el comando de consulta para obtener los tickets
	string consulta = "select * from registros_publicos where pendiente = 0 and fecha = '" + fecha + "'";

	// Realiza la consulta
	database.query( consulta );

	// ¿Hay resultados?
	if( results.size() > 0 ){
		for( auto row : results ){
			try{
				Registro *producto = productos.buscarRegistroPorClave( stoi( (* row)[ "clave_producto" ] ) );

				// Crea el nuevo ticket
				TicketPublico *ticket = new TicketPublico( row, producto );
    				
    		    // Lo agrega al campo de registros internos pendientes
    		    registros.push_back( ticket );
    	    }
    	    catch( invalid_argument &ia ){
    			cerr << ia.what() << endl;
    	    }
		}
	}
	else{
		app_mostrar_mensaje( "No se encontraron registros del día seleccionado." );
	}

	// Cierra la base de datos
	database.close();
}

// Actualiza los registros publicos pendientes
void publicoActualizarRegistros( list< TicketPublico * > &ticketsPublicos, std::string idContenedor )
{
    // Limpia el contenedor
	limpiar_contenedor( idContenedor );

    // Itera a través de la lista de tickets pendientes y crea los tickets necesarios
    for( list< TicketPublico * >::iterator ticket = ticketsPublicos.begin(); ticket != ticketsPublicos.end(); ticket++ ){
		// Crea un elemento que será añadido a la interfaz
		GtkBuilder *builder = gtk_builder_new();
		GError *error = NULL;

		stringstream clave;
		clave << setfill( '0' ) << setw( 7 ) << (*ticket) -> obtenerFolio();

		try{
			if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoTicketPublico.glade", &error ) != 0 ){
				gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "Ticket" ) ), to_string( (*ticket) -> obtenerFolio() ).c_str() );
				gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemEntradaFolioInterno" ) ), clave.str().c_str() );
				gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemEntradaFechaInterno" ) ), (*ticket) -> obtenerFecha().c_str() );
				gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemEntradaProductoInterno" ) ), (*ticket) -> obtenerProducto() -> obtenerNombre().c_str() );
				gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemEntradaPlacaInterno" ) ), (*ticket) -> obtenerNumeroPlacas().c_str() );

				gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( idContenedor ) ), GTK_WIDGET( buscar_objeto( "Ticket" ) ), (*ticket) -> obtenerFolio() );
			}
	    }
	    catch( runtime_error &re ){
		    cerr << re.what() << endl;
	    }

    }
}

// Eliminar el registro seleccionado
void publicoEliminarSeleccionado()
{
	// Elimina el registro de la base de datos
	database.open( databaseFile );

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
	app_mostrar_mensaje( "Registro eliminado" );

	// Cierra la ventana
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );

	// Redirige hacia la vista de consulta de registros
	publicoActualizarRegistros( registrosPublicosConsultados, "ContenedorRegistrosPesaje" );
	irHacia( nullptr, (void *)"Pesajes" );
}

// Manda a imprimir el registro interno seleccionado
void publicoImprimirSeleccionado()
{
	// Busca el ticket publico y lo imprime
	TicketPublico *registroPublico = buscarRegistroPublicoPorFolio( stoi( gtk_label_get_text( GTK_LABEL( buscar_objeto( "FolioPublico" ) ) ) ), registrosPublicosConsultados );
	if( registroPublico != nullptr ){
		registroPublico -> imprimir( empresa_razon_social );

		app_mostrar_mensaje( "Formato de impresión creado." );
	}
}

void publicoGenerarInforme()
{
	app_mostrar_mensaje( "Los informes para registros públicos\nno están disponibles." );
}
