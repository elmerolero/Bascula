#include "RegistrosInternos.h"
#include "GestorRegistros.h"
#include "LectorBascula.h"
#include "Aplicacion.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
using namespace std;

// Ticket a crear
Ticket *ticket = nullptr;

// Registra un nuevo ticket
void internoRegistrarPendiente()
{	
	// Obtiene la empresa y el producto introducidos
	string nombreEmpresa = interfaz.obtenerTextoEntrada( "EntradaNombreEmpresaInterno" );
	Registro *empresa = empresas.buscarRegistroPorNombre( nombreEmpresa );
	string nombreProducto = interfaz.obtenerTextoEntrada( "EntradaNombreProductoInterno" );
	Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );
	
	try{
		// Establece los datos obligatorios
		ticket -> establecerFolio( ++folioActual );
		ticket -> establecerFechaRegistro( interfaz.obtenerTextoEtiqueta( "EntradaFechaInterno" ) );
		ticket -> establecerEmpresa( empresa == nullptr ? empresas.agregarNuevoRegistro( nombreEmpresa ) : empresa );
		ticket -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		ticket -> establecerNombreConductor( interfaz.obtenerTextoEntrada( "EntradaNombreConductorInterno" ) );
		ticket -> establecerNumeroPlacas( interfaz.obtenerTextoEntrada( "EntradaNumeroPlacasInterno" ) );
		ticket -> establecerObservaciones( interfaz.obtenerTextoEntrada( "EntradaObservacionesInterno" ) );
		ticket -> establecerEntradaManual( lectorBascula.lecturaManualActivada() );
		
		// Lee el descuento
		if( ticket -> permitirDescuento() && !ticket -> estaDescuentoEstablecido() ){
			interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Indicó que se introduciría un descuento, pero el campo está vacío." );
			interfaz.mostrarElemento( "MensajeErrorCampo" );
			--folioActual;
			return;
		}
		
		// Al menos tiene que registrarse el peso bruto
		if( !ticket -> estaPesoBrutoEstablecido() ){
			interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Es necesario registrar el peso bruto." );
			interfaz.mostrarElemento( "MensajeErrorCampo" );
			--folioActual;
			return;
		}
		
		// Establece si está pendiente
		ticket -> establecerPendiente( !ticket -> estaPesoNetoCalculado() );
		
		// Establece el nombre del basculista que creó el registro
		ticket -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );
		
		crearRegistroPendiente( ticket );
		
		// Muestra el mensaje de que el ticket fue correctamente registrado
		mostrarMensaje( "Registro creado\ncorrectamente." );
		
		// Actualiza la vista de tickets
		internoActualizarRegistros( registrosInternosPendientes );
		
		// Establece las vistas
		irHacia( nullptr, (void *)"Tickets" );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		--folioActual;
	}
}

// Finaliza el ticket
void internoFinalizarPendiente()
{
	// ¿Está establecido el peso bruto?
	if( !ticket -> estaPesoBrutoEstablecido() ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Es necesario registrar el peso bruto." );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		return;
	}
	
	// ¿Está establecido el peso tara?
	if( !ticket -> estaPesoTaraEstablecido() ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Es necesario registrar el peso tara." );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		return;
	}
	
	// ¿Está habilitado el descuento?
	if( ticket -> permitirDescuento() && !ticket -> estaDescuentoEstablecido() ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Indicó que se introduciría un descuento, pero el campo está vacío." );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		--folioActual;
		return;
	}
	
	// ¿Está registrado el peso neto?
	if( !ticket -> estaPesoNetoCalculado() ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Es necesario calcular el peso neto." );
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
	
	// Actualiza la vista de tickets
	internoActualizarRegistros( registrosInternosPendientes );
		
	// Establece las vistas
	irHacia( nullptr, (void *)"Tickets" );
	
	// Muestra un mensaje de que fue finalizado correctamente
	mostrarMensaje( "Registro realizado correctamente." );
}

// Registra el peso bruto
void internoRegistrarPesoBruto()
{
	try{
		// Establece la hora y el peso bruto en el ticket
		ticket -> establecerPesoBruto( lectorBascula.leer() );
		ticket -> establecerHoraEntrada( obtenerHora() );
		
		interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoInterno", to_string( ticket -> obtenerPesoBruto() ) + " Kg" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaInterno", ticket -> obtenerHoraEntrada() );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoInterno", "No establecido" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaInterno", "No establecida" );
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
	}
	
	// Intenta calcula el peso neto
	internoCalcularPesoNeto();
	
	// Cierra el lector de la báscula
	lectorBascula.cerrar();
}

// Registra el peso tara
void internoRegistrarPesoTara()
{
	try{
		// Establece el peso tara y el peso de salida
		ticket -> establecerPesoTara( lectorBascula.leer() );
		ticket -> establecerHoraSalida( obtenerHora() );
		
		// Establece el peso leído en la etiqueta
		interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", to_string( ticket -> obtenerPesoTara() ) + " Kg" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", obtenerHora() );
		
		// Habilita la opción de poder finalizar el ticket
		interfaz.establecerBotonEtiqueta( "BotonRegistrarInterno", "Finalizar" );
	}
	catch( invalid_argument &ia ){
		// Reestablece el campo de la hora y el peso tara
		interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", "No establecido" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", "No establecida" );
		
		// Muestra el error que sucedió
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
	}
	
	internoLeerDescuento();
	internoCalcularPesoNeto();
	
	// Cierra el lector de la báscula
	lectorBascula.cerrar();
}

// Calcula el peso neto
void internoCalcularPesoNeto()
{
	//Intenta recalcular el peso neto
	try{
		ticket -> calcularPesoNeto();
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", to_string( ticket -> obtenerPesoNeto() ) + " Kg" );
		interfaz.establecerBotonEtiqueta( "BotonRegistrarInterno", "Finalizar" );
	}
	catch( invalid_argument &ia ){
		// No pasa nada, solo indicamos que no se estableció el peso neto e impedimos que lo registre como finalizado
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", "No establecido" );
		interfaz.establecerBotonEtiqueta( "BotonRegistrarInterno", "Pendiente" );
	}
}

// Habilita o deshabilita el descuento
void internoHabilitarDescuento()
{
	// Si ticket está establecido 
	if( ticket != nullptr ){
		if( interfaz.obtenerEstadoBotonToggle( "SiDescuentoInterno" ) ){
			ticket -> permitirDescuento( true );
			interfaz.habilitarEdicionEntrada( "EntradaDescuentoInterno" );
		}
		else{
			ticket -> permitirDescuento( false );
			interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", "" );
			interfaz.deshabilitarEdicionEntrada( "EntradaDescuentoInterno" );
		}
	
		// Intenta calcular el peso neto
		internoCalcularPesoNeto();
	}
}

// Calcula el descuento
void internoLeerDescuento()
{
	if( ticket -> permitirDescuento() ){
		try{
			// Establece el descuento
			ticket -> establecerDescuento( interfaz.obtenerTextoEntrada( "EntradaDescuentoInterno" ) );
			
			// Permite finalizar el ticket
			interfaz.establecerBotonEtiqueta( "BotonRegistrarInterno", "Finalizar" );
		}
		catch( invalid_argument &ia ){
			if( !ticket -> estaDescuentoEstablecido() ){
				interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
				interfaz.mostrarElemento( "MensajeErrorCampo" );
			}
		}
		
		// Intenta calcular el peso neto
		internoCalcularPesoNeto();
	}
}

// Establece el tipo de registro (ENTRADA o SALIDA)
void internoSeleccionarTipo()
{
	// Si el ticket está establecido
	if( ticket != nullptr ){
		if( interfaz.obtenerEstadoBotonToggle( "RegistraEntrada" ) ){
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_ENTRADA );
		}
		else{
			ticket -> establecerTipoRegistro( TIPO_REGISTRO_SALIDA );
		}
	}
}

void internoActualizarRegistros( list< Ticket * > &tickets )
{
	// Limpia el contenedor
	interfaz.removerElementosHijos( "ContenedorTickets" );
	
	// Itera a través de la lista de tickets pendientes y crea los tickets necesarios
	for( list< Ticket * >::iterator ticket = tickets.begin(); ticket != tickets.end(); ticket++ ){
        // Crea un elemento que será añadido a la interfaz
        Widget *elemento = new Widget();
        
        stringstream clave;
        clave << setfill( '0' ) << setw( 7 ) << (*ticket) -> obtenerFolio();
        
        try{
			elemento -> cargarWidget( "../resources/interfaces/ElementoTicket.glade" );
			elemento -> establecerTextoEtiqueta( "ItemEntradaFolioInterno", clave.str() );
			elemento -> establecerTextoEtiqueta( "ItemEntradaFechaInterno", (*ticket) -> obtenerFechaRegistro() );
			elemento -> establecerTextoEtiqueta( "ItemEntradaEmpresaInterno", (*ticket) -> obtenerEmpresa() -> obtenerNombre() );
			elemento -> establecerTextoEtiqueta( "ItemEntradaProductoInterno", (*ticket) -> obtenerProducto() -> obtenerNombre() );
			elemento -> establecerTextoEtiqueta( "ItemEntradaPlacaInterno", (*ticket) -> obtenerNumeroPlacas() );
			
			interfaz.insertarElementoAGrid( elemento, "Ticket", "ContenedorTickets", 0, (*ticket) -> obtenerFolio(), 1, 1 );
		}
		catch( runtime_error &re ){
			cerr << re.what() << endl;
		}
		
		delete elemento;
    }
}

// Cancela el registro
void internoCancelarRegistro()
{
	if( ticket != nullptr ){
		delete ticket;
		ticket = nullptr;
	}
	
	irHacia( nullptr, (void *)"Tickets" );
}

// Cancela la finalización
void internoCancelarFinalizacion()
{
	ticket = nullptr;
	
	irHacia( nullptr, (void *)"Tickets" );
}
