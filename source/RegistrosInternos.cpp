#include "RegistrosInternos.h"
#include "GestorRegistros.h"
#include "LectorBascula.h"
#include "Aplicacion.h"
#include "Funciones.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
using namespace std;

Ticket *ticket;

void internoRegistrarPendiente()
{
	try{
		// Bandera que indica si actualizar o registrar el nuevo ticket
		bool esRegistroNuevo = false;

		// Busca el ticket como pendiente, si no lo encuentra, crea uno
		Ticket * ticket = buscarRegistroInternoPorFolio( stoi( interfaz.obtenerTextoEtiqueta( "EntradaFolioInterno" ) ), registrosInternosPendientes );
		if( ticket == nullptr ){
			esRegistroNuevo = true;
			ticket = new Ticket();
			ticket -> establecerFolio( stoi( interfaz.obtenerTextoEtiqueta( "EntradaFolioInterno" ) ) );
		}

		// Obtiene la empresa y el producto introducidos
		string nombreEmpresa = interfaz.obtenerTextoEntrada( "EntradaNombreEmpresaInterno" );
		Registro *empresa = empresas.buscarRegistroPorNombre( nombreEmpresa );
		string nombreProducto = interfaz.obtenerTextoEntrada( "EntradaNombreProductoInterno" );
		Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );

		// Establece el resto de los datos
		ticket -> establecerFecha( interfaz.obtenerTextoEtiqueta( "EntradaFechaInterno" ) );
		ticket -> establecerEmpresa( empresa == nullptr ? empresas.agregarNuevoRegistro( nombreEmpresa ) : empresa );
		ticket -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		ticket -> establecerNombreConductor( interfaz.obtenerTextoEntrada( "EntradaNombreConductorInterno" ) );
		ticket -> establecerNumeroPlacas( interfaz.obtenerTextoEntrada( "EntradaNumeroPlacasInterno" ) );
		ticket -> establecerObservaciones( interfaz.obtenerTextoEntrada( "EntradaObservacionesInterno" ) );
		ticket -> establecerEntradaManual( lectorBascula.lecturaManualActivada() );
		ticket -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );
		ticket -> establecerPendiente( true );

		// Establece el tipo de registro
		if( interfaz.obtenerEstadoBotonToggle( "RegistraEntrada" ) ){
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_ENTRADA );
		}
		else{
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_SALIDA );
		}

		// Establece el peso bruto
		try{
			ticket -> establecerPesoBruto( interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoInterno" ) );
			ticket -> establecerHoraEntrada( interfaz.obtenerTextoEtiqueta( "EntradaHoraEntradaInterno" ) );
			ticket -> establecerPesoBrutoEstablecido( true );
		}
		catch( invalid_argument &ia ){
			ticket -> establecerPesoBruto( 0.f );
			ticket -> establecerHoraEntrada( "" );
			ticket -> establecerPesoBrutoEstablecido( false );
		}

		// Establece el peso tara
		try{ 
			ticket -> establecerPesoTara( interfaz.obtenerTextoEtiqueta( "EntradaPesoTaraInterno" ) );
			ticket -> establecerHoraSalida( interfaz.obtenerTextoEtiqueta( "EntradaHoraSalidaInterno" ) );
			ticket -> establecerPesoTaraEstablecido( true );
		}
		catch( invalid_argument &ia ){
			ticket -> establecerPesoTara( 0.f );
			ticket -> establecerHoraSalida( "" );
			ticket -> establecerPesoTaraEstablecido( false );
		}

		// Indica si está habilitado el descuento
		if( interfaz.obtenerEstadoBotonToggle( "SiDescuentoInterno" ) ){
			try{
				ticket -> permitirDescuento( true );
				ticket -> establecerDescuento( interfaz.obtenerTextoEntrada( "EntradaDescuentoInterno" ) );
				ticket -> establecerDescuentoEstablecido( true );
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
						ticket -> establecerPesoNeto( interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoInterno" ) );
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
					ticket -> establecerPesoNeto( interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoInterno" ) );
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

		// Crea o actualiza el ticket pendiente
		if( esRegistroNuevo ){
			crearRegistroPendiente( ticket );
			interfaz.establecerTextoEtiqueta( "MensajeTicketsPendientes", "Registro creado correctamente." );
			interfaz.mostrarElemento( "MensajeTicketsPendientes" );
		}
		else{
			actualizarRegistroPendiente( ticket );
			interfaz.establecerTextoEtiqueta( "MensajeTicketsPendientes", "Registro actualizado correctamente." );
			interfaz.mostrarElemento( "MensajeTicketsPendientes" );
		}

		// Actualiza la lista de registros
		internoActualizarRegistros( registrosInternosPendientes, "ContenedorTickets" );

		// Redirige hacia la vista de tickets
		irHacia( nullptr, (void *)"Tickets" );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
	}
}

void internoFinalizarPendiente()
{
	// Bandera que indica si actualizar o registrar el nuevo ticket
		bool esRegistroNuevo = false;

	// Busca el ticket como pendiente, si no lo encuentra, crea uno
	Ticket * ticket = buscarRegistroInternoPorFolio( stoi( interfaz.obtenerTextoEtiqueta( "EntradaFolioInterno" ) ), registrosInternosPendientes );

	try{
		if( ticket == nullptr ){
			esRegistroNuevo = true;
			ticket = new Ticket();
			ticket -> establecerFolio( stoi( interfaz.obtenerTextoEtiqueta( "EntradaFolioInterno" ) ) );
		}

		// Obtiene la empresa y el producto introducidos
		string nombreEmpresa = interfaz.obtenerTextoEntrada( "EntradaNombreEmpresaInterno" );
		Registro *empresa = empresas.buscarRegistroPorNombre( nombreEmpresa );
		string nombreProducto = interfaz.obtenerTextoEntrada( "EntradaNombreProductoInterno" );
		Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );

		// Establece el resto de los datos
		ticket -> establecerFecha( interfaz.obtenerTextoEtiqueta( "EntradaFechaInterno" ) );
		ticket -> establecerEmpresa( empresa == nullptr ? empresas.agregarNuevoRegistro( nombreEmpresa ) : empresa );
		ticket -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		ticket -> establecerNombreConductor( interfaz.obtenerTextoEntrada( "EntradaNombreConductorInterno" ) );
		ticket -> establecerNumeroPlacas( interfaz.obtenerTextoEntrada( "EntradaNumeroPlacasInterno" ) );
		ticket -> establecerObservaciones( interfaz.obtenerTextoEntrada( "EntradaObservacionesInterno" ) );
		ticket -> establecerEntradaManual( lectorBascula.lecturaManualActivada() );
		ticket -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );

		// Establece el tipo de registro
		if( interfaz.obtenerEstadoBotonToggle( "RegistraEntrada" ) ){
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_ENTRADA );
		}
		else{
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_SALIDA );
		}

		// Establece el peso bruto
		ticket -> establecerPesoBruto( interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoInterno" ) );
		ticket -> establecerHoraEntrada( interfaz.obtenerTextoEtiqueta( "EntradaHoraEntradaInterno" ) );
		ticket -> establecerPesoBrutoEstablecido( true );

		ticket -> establecerPesoTara( interfaz.obtenerTextoEtiqueta( "EntradaPesoTaraInterno" ) );
		ticket -> establecerHoraSalida( interfaz.obtenerTextoEtiqueta( "EntradaHoraSalidaInterno" ) );
		ticket -> establecerPesoTaraEstablecido( true );

		// Indica si está habilitado el descuento
		if( interfaz.obtenerEstadoBotonToggle( "SiDescuentoInterno" ) ){
			ticket -> permitirDescuento( true );
			string descuento = interfaz.obtenerTextoEntrada( "EntradaDescuentoInterno" );
			if( descuento.empty() ){
				// Indica un error
				interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Es necesario establecer un descuento." );
				interfaz.mostrarElemento( "MensajeErrorCampo" );

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
			ticket -> establecerDescuento( interfaz.obtenerTextoEntrada( "EntradaDescuentoInterno" ) );
			ticket -> establecerDescuentoEstablecido( true );
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
					ticket -> establecerPesoNeto( interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoInterno" ) );
					ticket -> establecerPesoNetoEstablecido( true );
				}
			}
			else{
				//Registra el peso neto
				ticket -> establecerPesoNeto( interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoInterno" ) );
				ticket -> establecerPesoNetoEstablecido( true );
			}
		}
		else{
			// No registra un peso neto
			interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Es necesario que el peso bruto, el peso tara se encuentren establecidos." );
			interfaz.mostrarElemento( "MensajeErrorCampo" );

			// Si es registro nuevo elimina el ticket
			if( esRegistroNuevo ){
				delete ticket;
				ticket = nullptr;
			}

			return;
		}

		// Registro correcto, ya no es un registro pendiente
		ticket -> establecerPendiente( false );

		// Finaliza el ticket pendiente
		finalizarRegistro( ticket );

		// Actualiza la lista de registros
		internoActualizarRegistros( registrosInternosPendientes, "ContenedorTickets" );

		// Muestra un mensaje que indica que finalizó adecuadamente
		interfaz.establecerTextoEtiqueta( "MensajeTicketsPendientes", "Registro finalizado. Se creó formato de impresión." );
		interfaz.mostrarElemento( "MensajeTicketsPendientes" );

		// Redirige hacia la vista de tickets
		irHacia( nullptr, (void *)"Tickets" );
	}
	catch( invalid_argument &ia ){
		/// Muetra un mensaje de error
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );

		// Si es registro nuevo elimina el ticket
		if( esRegistroNuevo ){
			delete ticket;
			ticket = nullptr;
		}
	}
}

// Registra el peso bruto
void internoRegistrarPesoBruto()
{
	// Cierra el lector de la báscula
	lectorBascula.cerrar();

	// Establece la hora y el peso bruto en el ticket
	try{
		interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoInterno", lectorBascula.leer() );
		interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaInterno", obtenerHora() );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoInterno", "No establecido" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaInterno", "No establecida" );
	}

	// Intenta calcula el peso neto
	internoActualizarPesoNeto();
}

// Registra el peso tara
void internoRegistrarPesoTara()
{
	// Cierra el lector de la báscula
	lectorBascula.cerrar();
		
	// Establece el peso leído en la etiqueta
	try{
		interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", lectorBascula.leer() );
		interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", obtenerHora() );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", "No establecido" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", "No establecida" );
	}

	internoActualizarPesoNeto();
}

// Actualiza el peso neto
void internoActualizarPesoNeto()
{
	try{
		// Establece peso bruto y tara
		double pesoBruto = stod( interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoInterno" ) );
		double pesoTara = stod( interfaz.obtenerTextoEtiqueta( "EntradaPesoTaraInterno" ) );
		double descuento;

		// Intenta obtener el descuento si este está habilitado
		if( interfaz.obtenerEstadoBotonToggle( "SiDescuentoInterno" ) ){
			descuento = stod( interfaz.obtenerTextoEntrada( "EntradaDescuentoInterno" ) );
		}
		else{
			descuento = 0.f;
		}

		// Establece el peso neto
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", pesoString( internoCalcularPesoNeto( pesoBruto, pesoTara, descuento ), 2 ) );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", "No establecido" );
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
	if( interfaz.obtenerEstadoBotonToggle( "RegistraEntrada" ) ){
		interfaz.establecerTextoEtiqueta( "EtiquetaEmpresa", "Proveedor:      " );
	}
	else{
		interfaz.establecerTextoEtiqueta( "EtiquetaEmpresa", "Cliente:           " );
	}
}

// Habilita o deshabilita el descuento
void internoHabilitarDescuento()
{
	// Si ticket está establecido 
	if( interfaz.obtenerEstadoBotonToggle( "SiDescuentoInterno" ) ){
		interfaz.habilitarEdicionEntrada( "EntradaDescuentoInterno" );
	}
	else{
		interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", "" );
		interfaz.deshabilitarEdicionEntrada( "EntradaDescuentoInterno" );
	}

	internoActualizarPesoNeto();
}

void internoActualizarRegistros( list< Ticket * > &tickets, std::string idContenedor )
{
	// Limpia el contenedor
	interfaz.removerElementosHijos( idContenedor );
	
	// Itera a través de la lista de tickets pendientes y crea los tickets necesarios
	for( list< Ticket * >::iterator ticket = tickets.begin(); ticket != tickets.end(); ticket++ ){
        // Crea un elemento que será añadido a la interfaz
        Widget *elemento = new Widget();
        
        stringstream clave;
        clave << setfill( '0' ) << setw( 7 ) << (*ticket) -> obtenerFolio();
        
        try{
			elemento -> cargarWidget( "../resources/interfaces/ElementoTicket.glade" );
			elemento -> establecerTextoEtiqueta( "ItemEntradaFolioInterno", clave.str() );
			elemento -> establecerTextoEtiqueta( "ItemEntradaFechaInterno", (*ticket) -> obtenerFecha() );
			elemento -> establecerTextoEtiqueta( "ItemEntradaEmpresaInterno", (*ticket) -> obtenerEmpresa() -> obtenerNombre() );
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

// Obtiene los registros de la fecha seleccionada
void internoObtenerPorFecha( list< Ticket * > &registros, std::string fecha )
{
	// Limpia la lista de los registros a consultar
	limpiarRegistrosInternos( registrosInternosConsultados );
	
	// Conecta con la base de datos
	database.open( nombreArchivo );

	// Genera el comando de consulta para obtener los tickets
	string consulta = "select * from registros_internos where pendiente = 0 and fecha = '" + fecha + "'";

	// Realiza la consulta
	database.query( consulta );

	// ¿Hay resultados?
	if( rows.size() > 0 ){
		for( size_t i = 0; i < rows.size(); i++ ){
			Row *row = rows[ i ];
			// Crea el nuevo ticket
			Ticket *ticket = new Ticket();

			try{
    		    // Establece los datos del registro
    		    establecerRegistroInternoDesdeRenglon( ticket, row );
    				
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

// Cancela el registro
void internoCancelar()
{
	irHacia( nullptr, (void *)"Tickets" );
}

// Manda a imprimir el registro interno seleccionado
void internoImprimirSeleccionado()
{
	if( ticket != nullptr ){
		ticket -> imprimir( nombreEmpresa, numeroFormatos, numeroCopias );

		mostrarMensaje( "Formato de impresión creado." );
	}
}

// Alerta de eliminacion
void internoAlertaEliminar()
{
	interfaz.establecerTextoEtiqueta( "MensajeAlerta", "¿Está seguro que desea eliminar este registro?\n"
													   "Una vez eliminado no se puede recuperar." );
	interfaz.mostrarElemento( "VentanaSiNo" );
}

// Eliminar el registro seleccionado
void internoEliminarSeleccionado()
{
	// Elimina el registro de la base de datos
	database.open( nombreArchivo );

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
	mostrarMensaje( "Registro eliminado" );

	// Cierra la ventana
	interfaz.ocultarElemento( "VentanaSiNo" );

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
	interfaz.obtenerFechaCalendario( "EntradaDiaConsultar", &dia, &mes, &anio );

	// Obtiene los tickets de la fecha seleccionada
	internoObtenerPorFecha( registrosInternosConsultados, obtenerFecha( dia, mes + 1, anio ) );

	// Establece la fecha del ticket que se está consultando
	interfaz.establecerTextoEtiqueta( "TicketsRegistrados", "Registros del día " + obtenerFecha( dia, mes, anio ) + ":"  );

	// Establece el número de tickets
	interfaz.establecerTextoEtiqueta( "TicketsContados", to_string( registrosInternosConsultados.size() ) + " registros"  );

	// Actualiza la lista de tickets
	internoActualizarRegistros( registrosInternosConsultados, "ContenedorRegistrosPesaje" );

	// Regresa a la vista anterior
	irHacia( nullptr, (void *)"Pesajes" );
}

void internoObtenerRegistrosRango()
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

    string consulta = "select folio, fecha, tipo_registro, nombre_empresa, nombre_producto, numero_placas, nombre_conductor, hora_entrada, "
    				  "hora_salida, peso_bruto, peso_tara, descuento, peso_neto, manual_activado, observaciones, nombre_basculista "
    				  "from registros_internos join productos on registros_internos.clave_producto = productos.clave_producto "
    				  "join empresas on registros_internos.clave_empresa = empresas.clave_empresa where pendiente = 0 and fecha between '" + 
    				  obtenerFecha( diaInicio, mesInicio + 1, anioInicio ) + "' and '" + obtenerFecha( diaFin, mesFin + 1, anioFin ) + "'";
    database.open( nombreArchivo );
    database.query( consulta );
    if( rows.size() > 0 ){
    	archivo << "Folio, Fecha, Registra, Empresa, Producto, No. Placas, Conductor, Hora entrada, Hora salida, Peso bruto, Peso tara, Descuento, Peso neto, Manual, Observaciones, Basculista" << endl;
    	for( Row *row : rows ){
			archivo << row -> columns.at( 0 ) << ", " << row -> columns.at( 1 ) << ", " << ( stoi( row -> columns.at( 2 ) ) == TIPO_REGISTRO_ENTRADA ? "Entrada" : "Salida" ) 
					<< ", " << row -> columns.at( 3 ) << ", " << row -> columns.at( 4 ) << ", " << row -> columns.at( 5 ) << ", " 
					<< row -> columns.at( 6 ) << ", " << row -> columns.at( 7 ) << ", " << row -> columns.at( 8 ) << ", " << row -> columns.at( 9 ) 
					<< ", " << row -> columns.at( 10 ) << ", " << row -> columns.at( 11 ) << ", " << row -> columns.at( 12 ) << ", " 
					<< row -> columns.at( 13 ) << ", " << row -> columns.at( 14 ) << ", " << row -> columns.at( 15 ) << endl;
    	}
    }
    database.close();
    archivo.close();

    ShellExecute(NULL, "open", "..\\resources\\data\\Registros.csv", NULL, NULL, SW_HIDE );
	irHacia( nullptr, (void *)"Pesajes" );
}

// Genera un informe
void internoGenerarInforme()
{
	// Muestra un mensaje que indica que se está generando el informe
	mostrarMensaje( "Generando informe.\nPor favor espere.");

	// Archivo con el informe
    ofstream archivo;

	// Se abre el archivo
    archivo.open( "../resources/data/Informe.csv", ios_base::out );
    if( !archivo ){
        mostrarMensaje( "No se pudo abrir el archivo.\n"
        				"Asegúrate de que ningún programa esté haciendo uso de el archivo,\n"
        				"de tener espacio en el disco duro e inténtalo de nuevo." );
        return;
    }

    // Establece el nombre de la empresa
    archivo << nombreEmpresa << endl
    		<< "Reportes de pesajes" << endl
    		<< "Fecha:, " << obtenerFecha() << endl;

    // Consulta a la base de datos
    string consulta = "select registros_internos.clave_empresa, nombre_empresa, productos.clave_producto, nombre_producto, sum( peso_bruto ), " 
    				  "sum( peso_tara ), sum( peso_neto ) from registros_internos "
					  "join empresas on registros_internos.clave_empresa = empresas.clave_empresa "
					  "join productos on registros_internos.clave_producto = productos.clave_producto "
					  "where tipo_registro = 0 and pendiente = 0 group by registros_internos.clave_empresa, "
					  "registros_internos.clave_producto order by nombre_empresa;";
    // Conecta con la base de datos
    database.open( nombreArchivo );
    database.query( consulta );
    if( rows.size() > 0 ){
		// Obtiene la clave de la primera empresa
    	unsigned int claveEmpresa = stoi( rows.at( 0 ) -> columns.at( 0 ) );

    	// Primer elemento
    	archivo << "Entradas" << endl
    			<< "Proveedor:," << rows.at(0) -> columns.at( 1 ) << endl
    			<< "Codigo, Producto, Total peso bruto, Total peso tara, Peso neto" << endl;

    	for( Row *row : rows ){
    		// ¿La clave empresa es diferente de la clave original?
    		if( claveEmpresa != stoi( row -> columns.at( 0 ) ) ){
    			// Establece la nueva clave de la empresa
    			claveEmpresa = stoi(  row -> columns.at( 0 ) );
    			archivo << endl
    				    << "Proveedor:," << row -> columns.at( 1 ) << endl
    					<< "Codigo, Producto, Total peso bruto, Total peso tara, Peso neto" << endl;
    		}

    		// Muestra los registros
    		archivo << row -> columns.at( 2 ) << ", " << row -> columns.at( 3 ) << ", " << row -> columns.at( 4 ) << ", " << row -> columns.at( 5 )
    				<< ", " << row -> columns.at( 6 ) << endl;
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
    	unsigned int claveEmpresa = stoi( rows.at( 0 ) -> columns.at( 0 ) );

    	// Primer elemento
    	archivo << "\nSalidas" << endl
    			<< "Proveedor:," << rows.at(0) -> columns.at( 1 ) << endl
    			<< "Codigo, Producto, Total peso bruto, Total peso tara, Peso neto" << endl;

    	for( Row *row : rows ){
    		// ¿La clave empresa es diferente de la clave original?
    		if( claveEmpresa != stoi( row -> columns.at( 0 ) ) ){
    			// Establece la nueva clave de la empresa
    			claveEmpresa = stoi(  row -> columns.at( 0 ) );
    			archivo << endl
    				    << "Proveedor:," << row -> columns.at( 1 ) << endl
    					<< "Codigo, Producto, Total peso bruto, Total peso tara, Peso neto" << endl;
    		}

    		// Muestra los registros
    		archivo << row -> columns.at( 2 ) << ", " << row -> columns.at( 3 ) << ", " << row -> columns.at( 4 ) << ", " << row -> columns.at( 5 )
    				<< ", " << row -> columns.at( 6 ) << endl;
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
    	unsigned int claveEmpresa = stoi( rows.at( 0 ) -> columns.at( 0 ) );

    	// Primer elemento
    	archivo << "\nTotales," << endl
    			<< "Codigo, Producto, Entrada, Salida, Neto" << endl;

    	for( Row *row : rows ){
    		// Muestra los registros
    		archivo << row -> columns.at( 0 ) << ", " << row -> columns.at( 1 ) << ", " << row -> columns.at( 2 ) << ", " << row -> columns.at( 3 )
    				<< ", " << row -> columns.at( 4 ) << endl;
    	}
    }

    database.close();
    archivo.close();

    ShellExecute(NULL, "open", "..\\resources\\data\\Informe.csv", NULL, NULL, SW_HIDE );

	irHacia( nullptr, (void *)"Pesajes" );
}
