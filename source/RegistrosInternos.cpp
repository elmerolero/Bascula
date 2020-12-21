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

// Ticket a crear
Ticket *ticket = nullptr;
bool internoPendiente = true;


Ticket *crearRegistroInterno()
{
	// Crea el nuevo ticket
	Ticket *ticket = new Ticket();
	if( ticket == nullptr ){
		throw runtime_error( "Hubo un error al crear el ticket." );
	}

	return ticket;
}

void internoRegistrarPendiente()
{
	// Ticket a crear
	Ticket *nuevo;

	try{
		// Crea el nuevo registro
		nuevo = crearRegistroInterno();

		// Obtiene la empresa y el producto introducidos
		string nombreEmpresa = interfaz.obtenerTextoEntrada( "EntradaNombreEmpresaInterno" );
		Registro *empresa = empresas.buscarRegistroPorNombre( nombreEmpresa );
		string nombreProducto = interfaz.obtenerTextoEntrada( "EntradaNombreProductoInterno" );
		Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );

		// Establece los datos obligatorios
		nuevo -> establecerFolio( ++folioActual );
		nuevo -> establecerFecha( interfaz.obtenerTextoEtiqueta( "EntradaFechaInterno" ) );
		nuevo -> establecerEmpresa( empresa == nullptr ? empresas.agregarNuevoRegistro( nombreEmpresa ) : empresa );
		nuevo -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		nuevo -> establecerNombreConductor( interfaz.obtenerTextoEntrada( "EntradaNombreConductorInterno" ) );
		nuevo -> establecerNumeroPlacas( interfaz.obtenerTextoEntrada( "EntradaNumeroPlacasInterno" ) );
		nuevo -> establecerObservaciones( interfaz.obtenerTextoEntrada( "EntradaObservacionesInterno" ) );
		nuevo -> establecerEntradaManual( lectorBascula.lecturaManualActivada() );

		// Lee el peso bruto
		nuevo -> establecerPesoBruto( interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoInterno" ) );
		nuevo -> establecerHoraEntrada( interfaz.obtenerTextoEtiqueta( "EntradaHoraEntradaInterno" ) );
		nuevo -> establecerPesoBrutoEstablecido( true );

		// Lee el peso tara
		if( !internoPendiente ){
			nuevo -> establecerPesoTara( interfaz.obtenerTextoEtiqueta( "EntradaPesoTaraInterno" ) );
			nuevo -> establecerHoraSalida( interfaz.obtenerTextoEtiqueta( "EntradaHoraSalidaInterno" ) );
			nuevo -> establecerPesoTaraEstablecido( true );
		}

		// Lee el descuento
		nuevo -> permitirDescuento( interfaz.obtenerEstadoBotonToggle( "SiDescuentoInterno" ) );
		if( nuevo -> permitirDescuento() ){
			internoActualizarPesoNeto();
			nuevo -> establecerDescuento( interfaz.obtenerTextoEntrada( "EntradaDescuentoInterno" ) );
			nuevo -> establecerDescuentoEstablecido( true );
		}
		else{
			nuevo -> establecerDescuento( 0.f );
			nuevo -> establecerDescuentoEstablecido( false );
		}

		// Intenta calcular el peso neto
		if( nuevo -> estaPesoBrutoEstablecido() && nuevo -> estaPesoTaraEstablecido() && !internoPendiente ){
			if( nuevo -> permitirDescuento() && nuevo -> estaDescuentoEstablecido() ){
				nuevo -> establecerPesoNeto( interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoInterno" ) );
				nuevo -> establecerPesoNetoEstablecido( true );
			}
			else{
				nuevo -> establecerPesoNeto( interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoInterno" ) );
				nuevo -> establecerPesoNetoEstablecido( true );
			}
		}
		else{
			nuevo -> establecerPesoNetoEstablecido( false );
		}

		// Registra el tipo de registro
		if( interfaz.obtenerEstadoBotonToggle( "RegistraEntrada" ) ){
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_ENTRADA );
		}
		else{
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_SALIDA );
		}

		// Nombre del basculista
		nuevo -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );

		// ¿Está pendiente?
		nuevo -> establecerPendiente( internoPendiente );
		
		// Crea el nuevo registros
		crearRegistroPendiente( nuevo );

		// Muestra el mensaje de que el nuevo registro fue creado de manera exitosa
		mostrarMensaje( "Registro creado exitosamente" );

		// Actualiza la vista de tickets
		internoActualizarRegistros( registrosInternosPendientes, "ContenedorTickets" );
		
		// Establece las vistas
		irHacia( nullptr, (void *)"Tickets" );
	}
	catch( invalid_argument &ia ){
		delete nuevo;
		nuevo = nullptr;
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		--folioActual;
	}
	catch( runtime_error re ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", re.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		--folioActual;
	}
}

void internoFinalizarPendiente()
{
	try{
		// Registra el nuevo peso tara
		ticket -> establecerPesoTara( interfaz.obtenerTextoEtiqueta( "EntradaPesoTaraInterno" ) );
		ticket -> establecerHoraSalida( interfaz.obtenerTextoEtiqueta( "EntradaHoraSalidaInterno" ) );
		ticket -> establecerPesoTaraEstablecido( true );

		// Registra el descuento final aplicado
		ticket -> permitirDescuento( interfaz.obtenerEstadoBotonToggle( "SiDescuentoInterno" ) );
		if( ticket -> permitirDescuento() ){
			internoActualizarPesoNeto();
			ticket -> establecerDescuento( interfaz.obtenerTextoEntrada( "EntradaDescuentoInterno" ) );
			ticket -> establecerDescuentoEstablecido( true );
		}
		else{
			ticket -> establecerDescuento( 0.f );
			ticket -> establecerDescuentoEstablecido( false );
		}

		// Registra el peso neto
		if( ticket -> estaPesoBrutoEstablecido() && ticket -> estaPesoTaraEstablecido() && !internoPendiente ){
			if( ticket -> permitirDescuento() && ticket -> estaDescuentoEstablecido() ){
				ticket -> establecerPesoNeto( interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoInterno" ) );
				ticket -> establecerPesoNetoEstablecido( true );
			}
			else{
				ticket -> establecerPesoNeto( interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoInterno" ) );
				ticket -> establecerPesoNetoEstablecido( true );
			}
		}
		else{
			interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Es necesario registrar todos los campos adecuadamente para establecer el peso neto." );
			interfaz.mostrarElemento( "MensajeErrorCampo" );
			return;
		}

		// Actualiza el tipo de registro
		if( interfaz.obtenerEstadoBotonToggle( "RegistraEntrada" ) ){
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_ENTRADA );
		}
		else{
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_SALIDA );
		}

		// Actualiza las observaciones
		ticket -> establecerObservaciones( interfaz.obtenerTextoEntrada( "EntradaObservacionesInterno" ) );
	
		// Se asegura que no esté pendiente
		ticket -> establecerPendiente( false );
	
		// Finaliza el registro
		finalizarRegistro( ticket );
	
		// Actualiza la vista de Tickets
		internoActualizarRegistros( registrosInternosPendientes, "ContenedorTickets" );
		
		// Establece las vistas
		irHacia( nullptr, (void *)"Tickets" );
	
		// Muestra un mensaje de que fue finalizado correctamente
		mostrarMensaje( "Registro finalizado correctamente.\nCreando formato de impresión." );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
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

double internoCalcularPesoNeto( double pesoBruto, double pesoTara, double descuento )
{
	return abs( pesoBruto - pesoTara ) - descuento;
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
	
	// Intenta calcular el peso neto
	internoActualizarPesoNeto();
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

// Calcula el peso neto
void internoActualizarPesoNeto()
{
	// Variables
	double pesoBruto;
	double pesoTara;
	double descuento;
	bool completo = true;

	// Intenta convertir el peso bruto leído
	try{
		pesoBruto = stod( interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoInterno" ) );
	}
	catch( invalid_argument &ia ){
		completo = false;
	}

	try{
		pesoTara = stod( interfaz.obtenerTextoEtiqueta( "EntradaPesoTaraInterno" ) );
	}
	catch( invalid_argument &ia ){
		completo = false;
	}

	try{
		if( interfaz.obtenerEstadoBotonToggle( "SiDescuentoInterno" ) ){
			descuento = stod( interfaz.obtenerTextoEntrada( "EntradaDescuentoInterno" ) );
			if( descuento > abs( pesoBruto - pesoTara ) ){
				mostrarMensaje( "El descuento introducido excede la diferencia de pesos.\nIntroduzca un descuento válido." );
				interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", "" );
				descuento = 0.f;
			}
		}
		else{
			descuento = 0.f;
		}
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", "" );
		completo = false;
	}

	// Intenta calculara el peso neto
	if( completo ){
		interfaz.ocultarElemento( "MensajeErrorCampo" );
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", pesoString( internoCalcularPesoNeto( pesoBruto, pesoTara, descuento ), 2 ) );
		interfaz.establecerBotonEtiqueta( "BotonRegistrarInterno", "Finalizar" );
		internoPendiente = false;
	}
	else{
		// No pasa nada, solo indicamos que no se estableció el peso neto e impedimos que lo registre como finalizado		
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", "No establecido" );
		interfaz.establecerBotonEtiqueta( "BotonRegistrarInterno", "Pendiente" );
		internoPendiente = true;
	}
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
		for( Row *row : rows ){
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
void internoCancelarRegistro()
{	
	irHacia( nullptr, (void *)"Tickets" );
}

// Cancela la finalización
void internoCancelarFinalizacion()
{
	irHacia( nullptr, (void *)"Tickets" );
}

// Manda a imprimir el registro interno seleccionado
void internoImprimirSeleccionado()
{
	if( ticket != nullptr ){
		ticket -> imprimir( nombreEmpresa, numeroCopias );

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
