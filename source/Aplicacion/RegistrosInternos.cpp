#include "RegistrosInternos.h"
#include "GestorRegistros.h"
#include "Aplicacion.h"
#include "Funciones.h"
#include "Vistas.h"
#include <stdexcept>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <cmath>
using namespace std;

Ticket *ticket;

void internoRegistrarPendiente()
{
	// Bandera que indica si actualizar o registrar el nuevo ticket
	bool esRegistroNuevo = false;
	
	// Ticket que será creado
	Ticket * ticket = nullptr;

	try{
		// Busca el ticket como pendiente, si no lo encuentra, crea uno
		ticket = buscarRegistroInternoPorFolio( stoi( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ) ) ), registrosInternosPendientes );
		if( ticket == nullptr ){
			folioActual++;
			ticket = new Ticket();
			esRegistroNuevo = true; 
			ticket -> establecerFolio( stoi( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ) ) ) );
		}

		// Obtiene la empresa y el producto introducidos
		string nombreEmpresa = gtk_entry_get_text( GTK_ENTRY( "EntradaNombreEmpresaInterno" ) );
		Registro *empresa = empresas.buscarRegistroPorNombre( nombreEmpresa );
		string nombreProducto = gtk_entry_get_text( GTK_ENTRY( "EntradaNombreProductoInterno" ) );
		Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );

		// Establece el resto de los datos
		ticket -> establecerFecha( gtk_label_get_text( GTK_LABEL( "EntradaFechaInterno" ) ) );
		ticket -> establecerEmpresa( empresa == nullptr ? empresas.agregarNuevoRegistro( nombreEmpresa ) : empresa );
		ticket -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		ticket -> establecerNombreConductor( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorInterno" ) ) ) );
		ticket -> establecerNumeroPlacas( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasInterno" ) ) ) );
		ticket -> establecerObservaciones( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaObservacionesInterno" ) ) ) );
		ticket -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );
		ticket -> establecerPendiente( true );

		// Establece el tipo de registro
		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "RegistraEntrada" ) ) ) ){
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_ENTRADA );
		}
		else{
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_SALIDA );
		}

		// Establece el peso bruto
		try{
			ticket -> establecerPesoBruto( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoInterno" ) ) ) );
			ticket -> establecerHoraEntrada( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaInterno" ) ) ) );
			ticket -> establecerPesoBrutoEstablecido( true );
		}
		catch( invalid_argument &ia ){
			ticket -> establecerPesoBruto( 0.f );
			ticket -> establecerHoraEntrada( "" );
			ticket -> establecerPesoBrutoEstablecido( false );
		}

		// Establece el peso tara
		try{
			ticket -> establecerPesoTara( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ) ) );
			ticket -> establecerHoraSalida( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaInterno" ) ) ) );
			ticket -> establecerPesoTaraEstablecido( true );
		}
		catch( invalid_argument &ia ){
			ticket -> establecerPesoTara( 0.f );
			ticket -> establecerHoraSalida( "" );
			ticket -> establecerPesoTaraEstablecido( false );
		}

		// Indica si está habilitado el descuento
		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "SiDescuentoInterno" ) ) ) ){
			try{
				ticket -> permitirDescuento( true );
				ticket -> establecerDescuento( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ) ) );
				ticket -> establecerDescuentoEstablecido( true );

				// Actualiza el peso neto
				internoActualizarPesoNeto();
			}
			catch( invalid_argument &ia ){
				ticket -> establecerDescuento( 0.f );
				ticket -> establecerDescuentoEstablecido( false );
			}
		}
		else{
			ticket -> permitirDescuento( false );
			ticket -> establecerDescuento( 0.f );
			ticket -> establecerDescuentoEstablecido( false );
		}

		// Intenta calcular el peso neto
		try{
			// ¿Están el peso bruto y el peso tara establecidos?
			if( ticket -> estaPesoBrutoEstablecido() && ticket -> estaPesoTaraEstablecido() ){
				// ¿Permite descuento?
				if( ticket -> permitirDescuento() ){
					// ¿Está establecido el descuento?
					if( ticket -> estaDescuentoEstablecido() ){
						//Registra el peso neto
						ticket -> establecerPesoNeto( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaPesoNetoInterno" ) ) ) );
						ticket -> establecerPesoNetoEstablecido( true );
					}
					else{
						// Establece el peso neto a cero e indica que no se estableció un peso neto
						ticket -> establecerPesoNeto( 0.f );
						ticket -> establecerPesoNetoEstablecido( false );
					}
				}
				else{
					//Registra el peso neto
					ticket -> establecerPesoNeto( gtk_label_get_text( GTK_LABEL( "EntradaPesoNetoInterno" ) ) );
					ticket -> establecerPesoNetoEstablecido( true );
				}
			}
			else{
				// No registra un peso neto
				ticket -> establecerPesoNeto( 0.f );
				ticket -> establecerPesoNetoEstablecido( false );
			}
		}
		catch( invalid_argument &ia ){
			// No guarda un peso neto
			ticket -> establecerPesoNeto( 0.f );
			ticket -> establecerPesoNetoEstablecido( false );
		}

		// Redirige hacia la vista de tickets
		regresarVista();

		// Crea o actualiza el ticket pendiente
		if( esRegistroNuevo ){
			crearRegistroPendiente( ticket );
			app_mostrar_error( "Registro creado correctamente." );
		}
		else{
			actualizarRegistroPendiente( ticket );
			app_mostrar_error( "Registro actualizado correctamente." );
		}

		// Actualiza la lista de registros
		internoActualizarRegistros( registrosInternosPendientes, "ContenedorTickets" );
	}
	catch( invalid_argument &ia ){
		// Regresa al folio anterior
		folioActual--;
	
		// Muestra un mensaje de error al usuario
		app_mostrar_error( ia.what() );

		// Si se estaba creando un ticket erróneo se elimina
		if( esRegistroNuevo && ticket != nullptr ){
			delete ticket;
		}
	}
}

void internoFinalizarPendiente()
{
	// Bandera que indica si actualizar o registrar el nuevo ticket
	bool esRegistroNuevo = false;

	// Busca el ticket como pendiente, si no lo encuentra, crea uno
	Ticket * ticket = buscarRegistroInternoPorFolio( stoi( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ) ) ), registrosInternosPendientes );

	try{
		if( ticket == nullptr ){
			folioActual++;
			ticket = new Ticket();
			esRegistroNuevo = true;
			ticket -> establecerFolio( stoi( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ) ) ) );
		}

		// Obtiene la empresa y el producto introducidos
		string nombreEmpresa = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreEmpresaInterno" ) ) );
		Registro *empresa = empresas.buscarRegistroPorNombre( nombreEmpresa );
		string nombreProducto = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoInterno" ) ) );
		Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );

		// Establece el resto de los datos
		ticket -> establecerFecha( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFechaInterno" ) ) ) );
		ticket -> establecerEmpresa( empresa == nullptr ? empresas.agregarNuevoRegistro( nombreEmpresa ) : empresa );
		ticket -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		ticket -> establecerNombreConductor( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorInterno" ) ) ) );
		ticket -> establecerNumeroPlacas( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasInterno" ) ) ) );
		ticket -> establecerObservaciones( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaObservacionesInterno" ) ) ) );
		ticket -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );

		// Establece el tipo de registro
		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "RegistraEntrada" ) ) ) ){
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_ENTRADA );
		}
		else{
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_SALIDA );
		}

		// Establece el peso bruto
		ticket -> establecerPesoBruto( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoInterno" ) ) ) );
		ticket -> establecerHoraEntrada( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaInterno" ) ) ) );
		ticket -> establecerPesoBrutoEstablecido( true );

		// Registra el peso tara
		ticket -> establecerPesoTara( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ) ) );
		ticket -> establecerHoraSalida( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaInterno") ) ) );
		ticket -> establecerPesoTaraEstablecido( true );

		// Indica si está habilitado el descuento
		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "SiDescuentoInterno" ) ) ) ){
			ticket -> permitirDescuento( true );
			
			string descuento = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ) );
			if( descuento.empty() ){
				// Indica un error
				app_mostrar_error( "Es necesario establecer un descuento." );

				// Establece los parámetros para descuento
				ticket -> establecerDescuento( 0.f );
				ticket -> establecerDescuentoEstablecido( false );

				// Si es registro nuevo elimina el ticket
				if( esRegistroNuevo ){
					delete ticket;
					ticket = nullptr;
				}

				return;
			}

			// Establece el descuento
			ticket -> establecerDescuento( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ) ) );
			ticket -> establecerDescuentoEstablecido( true );

			// Intenta actualizar el peso neto
			internoActualizarPesoNeto();
		}
		else{
			// No permite descuento
			ticket -> permitirDescuento( false );
			ticket -> establecerDescuento( 0.f );
			ticket -> establecerDescuentoEstablecido( false );
		}

		// ¿Están el peso bruto y el peso tara establecidos?
		if( ticket -> estaPesoBrutoEstablecido() && ticket -> estaPesoTaraEstablecido() ){
			// ¿Permite descuento?
			if( ticket -> permitirDescuento() ){
				// ¿Está establecido el descuento?
				if( ticket -> estaDescuentoEstablecido() ){
					//Registra el peso neto
					ticket -> establecerPesoNeto( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno" ) ) ) );
					ticket -> establecerPesoNetoEstablecido( true );
				}
			}
			else{
				//Registra el peso neto
				ticket -> establecerPesoNeto( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno"  ) ) ) );
				ticket -> establecerPesoNetoEstablecido( true );
			}
		}
		else{
			// No registra un peso neto
			app_mostrar_error( "Es necesario que el peso bruto y el peso tara se encuentren establecidos." );

			// Si es registro nuevo elimina el ticket
			if( esRegistroNuevo ){
				delete ticket;
				ticket = nullptr;
				folioActual--;
			}

			return;
		}

		// Registro correcto, ya no es un registro pendiente
		ticket -> establecerPendiente( false );

		// Finaliza el ticket pendiente
		finalizarRegistro( ticket, esRegistroNuevo );

		// Actualiza la lista de registros
		internoActualizarRegistros( registrosInternosPendientes, "ContenedorTickets" );

		// Redirige hacia la vista de tickets
		regresarVista();

		// Muestra un mensaje que indica que finalizó adecuadamente
		app_mostrar_error( "Registro finalizado. Se creará formato de impresión si la opción se encuentra habilitada." );
	}
	catch( invalid_argument &ia ){
		/// Muestra un mensaje de error
		app_mostrar_error( ia.what() );

		// Si es registro nuevo elimina el ticket
		if( esRegistroNuevo ){
			delete ticket;
			ticket = nullptr;
			folioActual--;
		}
	}
}

// Registra el peso bruto
void internoRegistrarPesoBruto()
{
	// Registra el peso bruto
	//basculaRegistrarPeso( "EntradaPesoBrutoInterno", "EntradaHoraEntradaInterno" );

	// Intenta calcula el peso neto
	internoActualizarPesoNeto();
}

// Registra el peso tara
void internoRegistrarPesoTara()
{
	// Registra el peso tara
	//basculaRegistrarPeso( "EntradaPesoTaraInterno", "EntradaHoraSalidaInterno" );

	// Intenta calcular el peso neto
	internoActualizarPesoNeto();
}

// Actualiza el peso neto
void internoActualizarPesoNeto()
{
	try{
		// Establece peso bruto y tara
		double pesoBruto = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoInterno" ) ) ) );
		double pesoTara = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ) ) );
		double descuento;

		// Intenta obtener el descuento si este está habilitado
		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "SiDescuentoInterno" ) ) ) ){
			descuento = stod( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ) ) );
		}
		else{
			descuento = 0.f;
		}

		// Establece el peso neto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno" ) ), pesoString( internoCalcularPesoNeto( pesoBruto, pesoTara, descuento ), 2 ).c_str() );
	}
	catch( invalid_argument &ia ){
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno" ) ), "No establecido" );
	}
}

double internoCalcularPesoNeto( double pesoBruto, double pesoTara, double descuento )
{
	return abs( pesoBruto - pesoTara ) - descuento;
}

// Establece el tipo de registro (ENTRADA o SALIDA)
void internoSeleccionarTipo()
{
	// Si el ticket está establecido
	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "RegistraEntrada" ) ) ) ){
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaEmpresa" ) ), "Proveedor:      " );
	}
	else{
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaEmpresa" ) ), "Cliente:           " );
	}
}

// Habilita o deshabilita el descuento
void internoHabilitarDescuento()
{
	// Si ticket está establecido 
	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "SiDescuentoInterno" ) ) ) ){
		gtk_editable_set_editable( GTK_EDITABLE( buscar_objeto( "EntradaDescuentoInterno" ) ), TRUE );
	}
	else{
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ), "" );
		gtk_editable_set_editable( GTK_EDITABLE( buscar_objeto( "EntradaDescuentoInterno" ) ), FALSE );
	}

	internoActualizarPesoNeto();
}

void internoActualizarRegistros( list< Ticket * > &tickets, std::string idContenedor )
{
	// Limpia el contenedor
	limpiar_contenedor( idContenedor );
	
	// Itera a través de la lista de tickets pendientes y crea los tickets necesarios
	for( list< Ticket * >::iterator ticket = tickets.begin(); ticket != tickets.end(); ticket++ ){
        // Crea un elemento que será añadido a la interfaz
        GError *error = NULL;
        GtkBuilder *builder = gtk_builder_new();
        
        stringstream clave;
        clave << setfill( '0' ) << setw( 7 ) << (*ticket) -> obtenerFolio();
        
        try{
			if( gtk_builder_add_from_file( builder , "../recursos/interfaces/ElementoTicket.glade", &error ) != 0 ){
				gtk_widget_set_name( GTK_WIDGET( buscar_objeto( "Ticket" ) ), to_string( (* ticket) -> obtenerFolio() ).c_str() );
				gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemEntradaFolioInterno" ) ), clave.str().c_str() );
				gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemEntradaFechaInterno" ) ), (*ticket) -> obtenerFecha().c_str() );
				gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemEntradaEmpresaInterno" ) ), (*ticket) -> obtenerEmpresa() -> obtenerNombre().c_str() );
				gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemEntradaProductoInterno" ) ), (*ticket) -> obtenerProducto() -> obtenerNombre().c_str() );
				gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemEntradaPlacaInterno" ) ), (*ticket) -> obtenerNumeroPlacas().c_str() );
			}
			
			gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( idContenedor ) ), GTK_WIDGET( buscar_objeto( "Ticket" ) ), (* ticket) -> obtenerFolio() );
		}
		catch( runtime_error &re ){
			cerr << re.what() << endl;
		}
		
		//delete elemento;
    }
}

// Obtiene los registros de la fecha seleccionada
void internoObtenerPorFecha( list< Ticket * > &registros, std::string fecha )
{
	// Limpia la lista de los registros a consultar
	limpiarRegistrosInternos( registrosInternosConsultados );
	
	// Conecta con la base de datos
	database.open( databaseFile );

	// Genera el comando de consulta para obtener los tickets
	string consulta = "select * from registros_internos where pendiente = 0 and fecha = '" + fecha + "'";

	// Realiza la consulta
	database.query( consulta );

	// ¿Hay resultados?
	if( results.size() > 0 ){
		for( auto renglon : results ){
			try{
				//
				Registro *producto = productos.buscarRegistroPorClave( stoi( (* renglon)[ "clave_producto" ] ) );
				Registro *empresa = empresas.buscarRegistroPorClave( stoi( (* renglon)[ "clave_empresa" ] ) );

				// Crea el nuevo ticket
				Ticket *ticket = new Ticket( renglon, producto, empresa );
    				
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

// Cancela el registro
void internoCancelar()
{
	// Redirige hacia la vista de tickets
	irHacia( nullptr, (void *)"Tickets" );

	// Oculta la barra de mensajes
	gtk_widget_hide( GTK_WIDGET( "MensajeTicketsPendientes" ) );
}

// Manda a imprimir el registro interno seleccionado
void internoImprimirSeleccionado()
{
	// Obtiene el ticket que se desea consultar
	Ticket *ticket = buscarRegistroInternoPorFolio( stoi( gtk_label_get_text( GTK_LABEL( buscar_objeto( "FolioInterno" ) ) ) ), registrosInternosConsultados );
	if( ticket != nullptr ){
		ticket -> imprimir( empresa_razon_social, numeroFormatos, numeroCopias );

		app_mostrar_mensaje( "Formato de impresión creado." );
	}
}

// Alerta de eliminacion
void internoAlertaEliminar()
{
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "MensajeAlerta" ) ), "¿Está seguro que desea eliminar este registro?\n"
																		"Una vez eliminado no se puede recuperar." );
	gtk_widget_show( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );						   
}

// Eliminar el registro seleccionado
void internoEliminarSeleccionado()
{
	// Elimina el registro de la base de datos
	database.open( databaseFile );

	// Comando de consulta
	string consulta = "delete from registros_internos where folio = " + to_string( ticket -> obtenerFolio() );
	database.query( consulta );

	database.close();

	// Elimina el registro de la lista
	if( ticket != nullptr ){
		registrosInternosConsultados.remove( ticket );
		delete ticket;
		ticket = nullptr;
	}
	else{
		cerr << "Alerta, intento de eliminación de un registro nulo." << endl;
	}

	// Muestra un mensaje que indica que el registro fue eliminado correctamente
	app_mostrar_mensaje( "Registro eliminado" );

	// Cierra la ventana
	gtk_widget_hide( GTK_WIDGET( "VentanaSiNo" ) );

	// Redirige hacia la vista de consulta de registros
	internoActualizarRegistros( registrosInternosConsultados, "ContenedorRegistrosPesaje" );
	irHacia( nullptr, (void *)"Pesajes" );
}

// Obtiene los tickets del día seleccionado
void internoSeleccionarDia()
{
	// Variables de la fecha del dia seleccionado
	unsigned int dia, mes, anio;

	// Obtiene la fecha
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "EntradaDiaConsultar" ) ), &dia, &mes, &anio );

	// Obtiene los tickets de la fecha seleccionada
	internoObtenerPorFecha( registrosInternosConsultados, tiempo_construir_fecha( dia, mes + 1, anio ) );

	// Establece la fecha del ticket que se está consultando
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "TicketsRegistrados" ) ), ("Registros del día " + tiempo_construir_fecha( dia, mes, anio ) + ":").c_str() );

	// Establece el número de tickets
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "TicketsContados" ) ), (to_string( registrosInternosConsultados.size() ) + " registros").c_str() );

	// Actualiza la lista de tickets
	internoActualizarRegistros( registrosInternosConsultados, "ContenedorRegistrosPesaje" );

	// Regresa a la vista anterior
	irHacia( nullptr, (void *)"Pesajes" );
}

void internoObtenerRegistrosRango()
{
	// Muestra un mensaje que indica que se está generando el informe
	app_mostrar_mensaje( "Obteniendo registros.\nPor favor espere.");

	// Variables para el rango de seleccion de fechas
	unsigned int diaInicio, mesInicio, anioInicio;
	unsigned int diaFin, mesFin, anioFin;

	// Obtiene las fechas
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "FechaInformeInicio" ) ), &diaInicio, &mesInicio, &anioInicio );
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "FechaInformeInicio" ) ), &diaFin, &mesFin, &anioFin );

	// Consulta de la base de datos
    string consulta = "select folio, fecha, tipo_registro, nombre_empresa, nombre_producto, numero_placas, nombre_conductor, hora_entrada, "
    				  "hora_salida, peso_bruto, peso_tara, descuento, peso_neto, manual_activado, observaciones, nombre_basculista "
    				  "from registros_internos join productos on registros_internos.clave_producto = productos.clave_producto "
    				  "join empresas on registros_internos.clave_empresa = empresas.clave_empresa where pendiente = 0 and fecha between '" + 
    				  tiempo_construir_fecha( diaInicio, mesInicio + 1, anioInicio ) + "' and '" + tiempo_construir_fecha( diaFin, mesFin + 1, anioFin ) + "'";
    database.open( databaseFile );
    database.query( consulta, "../resources/data/Registros.csv" );
    database.close();

    ShellExecute(NULL, "open", "..\\resources\\data\\Registros.csv", NULL, NULL, SW_HIDE );
	irHacia( nullptr, (void *)"Pesajes" );
}

// Genera un informe
void internoGenerarInforme()
{
/*
	// Muestra un mensaje que indica que se está generando el informe
	app_mostrar_mensaje( "Generando informe.\nPor favor espere.");

	// Archivo con el informe
    ofstream archivo;

	// Se abre el archivo
    archivo.open( "../resources/data/Informe.csv", ios_base::out );
    if( !archivo ){
        app_mostrar_mensaje( "No se pudo abrir el archivo.\n"
        				"Asegúrate de que ningún programa esté haciendo uso de el archivo,\n"
        				"de tener espacio en el disco duro e inténtalo de nuevo." );
        return;
    }
	
	// Agrega el caracter BOM para archivos UTF-8
	const char *bom = "\xef\xbb\xbf";
    archivo << bom;

    // Establece el nombre de la empresa
    archivo << nombreEmpresa << endl
    		<< "Reportes de pesajes" << endl
    		<< "Fecha:, " << tiempo_obtener_fecha_corta() << endl;

    // Consulta a la base de datos
    string consulta = "select registros_internos.clave_empresa as clave_empresa, nombre_empresa, productos.clave_producto, nombre_producto, sum( peso_bruto ), " 
    				  "sum( peso_tara ), sum( peso_neto ) from registros_internos "
					  "join empresas on registros_internos.clave_empresa = empresas.clave_empresa "
					  "join productos on registros_internos.clave_producto = productos.clave_producto "
					  "where tipo_registro = 0 and pendiente = 0 group by registros_internos.clave_empresa, "
					  "registros_internos.clave_producto order by nombre_empresa;";

    // Conecta con la base de datos
    database.open( databaseFile );
    database.query( consulta );
    if( results.size() > 0 ){
		// Obtiene la clave de la primera empresa
    	unsigned int claveEmpresa = stoi( (* results.at( 0 ))[ "clave_empresa" ] );

    	// Primer elemento
    	archivo << "Entradas" << endl
    			<< "Proveedor:, " << (* results.at( 0 ))[ "nombre_empresa" ] << endl
    			<< "Codigo, Producto, Total peso bruto, Total peso tara, Peso neto" << endl;

    	for( auto *row : results ){
    		// ¿La clave empresa es diferente de la clave original?
    		if( claveEmpresa != stoi( (* row )[ "clave_empresa" ] ) ){
    			// Establece la nueva clave de la empresa
    			claveEmpresa = stoi(  (* row )[ "clave_empresa" ] );
    			archivo << endl
    				    << "Proveedor:," << (* row )[ "nombre_empresa" ] << endl
    					<< "Codigo, Producto, Total peso bruto, Total peso tara, Peso neto" << endl;
    		}

    		// Muestra los registros
    		archivo << row -> at( 2 ) << ", " << row -> at( 3 ) << ", " << row -> at( 4 ) << ", " << row -> at( 5 )
    				<< ", " << row -> at( 6 ) << endl;
    	}
    }

    // Consulta a la base de datos
    consulta = "select registros_internos.clave_empresa, nombre_empresa, productos.clave_producto, nombre_producto, sum( peso_bruto ), " 
    		   "sum( peso_tara ), sum( peso_neto ) from registros_internos "
			   "join empresas on registros_internos.clave_empresa = empresas.clave_empresa "
			   "join productos on registros_internos.clave_producto = productos.clave_producto "
			   "where tipo_registro = 1 and pendiente = 0 "
			   "group by registros_internos.clave_empresa, registros_internos.clave_producto order by nombre_empresa;";
	database.query( consulta );
	if( rows.size() > 0 ){
		// Obtiene la clave de la primera empresa
    	unsigned int claveEmpresa = stoi( rows.at( 0 ) -> at( 0 ) );

    	// Primer elemento
    	archivo << "\nSalidas" << endl
    			<< "Proveedor:," << rows.at(0) -> at( 1 ) << endl
    			<< "Codigo, Producto, Total peso bruto, Total peso tara, Peso neto" << endl;

    	for( Row *row : rows ){
    		// ¿La clave empresa es diferente de la clave original?
    		if( claveEmpresa != stoi( row -> at( 0 ) ) ){
    			// Establece la nueva clave de la empresa
    			claveEmpresa = stoi(  row -> at( 0 ) );
    			archivo << endl
    				    << "Proveedor:," << row -> at( 1 ) << endl
    					<< "Codigo, Producto, Total peso bruto, Total peso tara, Peso neto" << endl;
    		}

    		// Muestra los registros
    		archivo << row -> at( 2 ) << ", " << row -> at( 3 ) << ", " << row -> at( 4 ) << ", " << row -> at( 5 )
    				<< ", " << row -> at( 6 ) << endl;
    	}
    }

    consulta = "select productos.clave_producto, nombre_producto, sum( case when tipo_registro = 0 then peso_neto else 0 end ) "
    		   "as entrada, sum( case when tipo_registro = 1 then peso_neto else 0 end ) as salida, ( sum( case when tipo_registro = 0 "
    		   "then peso_neto else 0 end ) - sum( case when tipo_registro = 1 then peso_neto else 0 end ) ) as total from registros_internos "
    		   "join productos on registros_internos.clave_producto = productos.clave_producto where pendiente = 0 group by registros_internos.clave_producto "
    		   "order by nombre_producto;";
    database.query( consulta );
	if( rows.size() > 0 ){
		// Obtiene la clave de la primera empresa
    	unsigned int claveEmpresa = stoi( rows.at( 0 ) -> at( 0 ) );

    	// Primer elemento
    	archivo << "\nTotales," << endl
    			<< "Codigo, Producto, Entrada, Salida, Neto" << endl;

    	for( Row *row : rows ){
    		// Muestra los registros
    		archivo << row -> at( 0 ) << ", " << row -> at( 1 ) << ", " << row -> at( 2 ) << ", " << row -> at( 3 )
    				<< ", " << row -> at( 4 ) << endl;
    	}
    }

    database.close();
    archivo.close();

    ShellExecute(NULL, "open", "..\\resources\\data\\Informe.csv", NULL, NULL, SW_HIDE );
*/
	irHacia( nullptr, (void *)"Pesajes" );
}
