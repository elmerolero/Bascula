#include "RegistrosPublicos.h"
#include "Aplicacion.h"
#include "LectorBascula.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
using namespace std;

TicketPublico *registroPublico = nullptr;

// Registra un ticket pendiente
void publicoRegistrarPendiente()
{
    // Obtiene el producto introducido
    string nombreProducto = interfaz.obtenerTextoEntrada( "EntradaNombreProductoPublico" );
    Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );
    
    try{
	registroPublico -> establecerFolio( ++folioActualPublico );
	registroPublico -> establecerFechaRegistro( interfaz.obtenerTextoEtiqueta( "EntradaFechaPublico" ) );
	registroPublico -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
	registroPublico -> establecerNombreConductor( interfaz.obtenerTextoEntrada( "EntradaNombreConductorPublico" ) );
	registroPublico -> establecerNumeroPlacas( interfaz.obtenerTextoEntrada( "EntradaNumeroPlacasPublico" ) );
	registroPublico -> establecerEntradaManual( lectorBascula.lecturaManualActivada() );
	
	// ¿Se registró al menos el peso bruto?
	if( !registroPublico -> estaPesoBrutoEstablecido() ){
	    interfaz.establecerTextoEtiqueta( "MensajeErrorCampoPublico", "Es necesario registrar el peso bruto." );
	    interfaz.mostrarElemento( "MensajeErrorCampoPublico" );
	    --folioActualPublico;
	    return;
	}
	
	// Establece si está pendiente
	registroPublico -> establecerPendiente( !registroPublico -> estaPesoNetoEstablecido() );
	
	// Establece el nombre de la persona que generó el ticket
	registroPublico -> establecerNombreBasculista( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() );
	
	crearRegistroPublicoPendiente( registroPublico );
	
	// Muestra mensaje de que el registro fue correctamente registrado
	mostrarMensaje( "Registro creado\ncorrectamente." );
	
	// Actualiza la vista de tickets
	publicoActualizarRegistros( registrosPublicosPendientes );
		
	// Establece las vistas
	irHacia( nullptr, (void *)"Tickets" );
    }
    catch( invalid_argument &ia ){
	folioActualPublico--;
	interfaz.establecerTextoEtiqueta( "MensajeErrorCampoPublico", ia.what() );
	interfaz.mostrarElemento( "MensajeErrorCampoPublico" );
    }
}

// Finalizar ticket
void publicoFinalizarPendiente()
{
    // ¿Está establecido el peso bruto?
    if( !registroPublico -> estaPesoBrutoEstablecido() ){
	interfaz.establecerTextoEtiqueta( "MensajeErrorCampoPublico", "Es necesario establecer el peso bruto." );
	interfaz.mostrarElemento( "MensajeErrorCampoPublico" );
	return;
    }
    
    // ¿Está establecido el peso tara?
    if( !registroPublico -> estaPesoTaraEstablecido() ){
	interfaz.establecerTextoEtiqueta( "MensajeErrorCampoPublico", "Es necesario indicar el peso tara." );
	interfaz.mostrarElemento( "MensajeErrorCampoPublico" );
	return;
    }
    
    // ¿Está registrado el peso neto?
    if( !registroPublico -> estaPesoNetoEstablecido() ){
	interfaz.establecerTextoEtiqueta( "MensajeErrorCampoPublico", "Es necesario calcular el peso neto." );
	interfaz.mostrarElemento( "MensajeErrorCampoPublico" );
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
    publicoActualizarRegistros( registrosPublicosPendientes );
		
    // Establece las vistas
    irHacia( nullptr, (void *)"Tickets" );
	
    // Muestra un mensaje de que fue finalizado correctamente
    mostrarMensaje( "Registro realizado correctamente." );
}

// Registra el peso bruto
void publicoRegistrarPesoBruto()
{
    try{
    	// Cierra la báscula
   		lectorBascula.cerrar();
   		
		// Establece la hora y el peso bruto del ticket
		registroPublico -> establecerPesoBruto( lectorBascula.leer() );
		registroPublico -> establecerHoraEntrada( obtenerHora() );
		
		// Indica en la interfaz los datos registrados
		interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoPublico", to_string( registroPublico -> obtenerPesoBruto() ) + " Kg" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaPublico", registroPublico -> obtenerHoraEntrada() );
    }
    catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoPublico", "No establecido" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaPublico", "No establecida" );
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
    }
    
    // Intenta calcular el peso neto
    publicoCalcularPesoNeto();
}

// Registra el peso tara
void publicoRegistrarPesoTara()
{
    try{
    	// Cierra el lector de la báscula
    	lectorBascula.cerrar();
	
		// Establece el peso tara y la hora de salida
		registroPublico -> establecerPesoTara( lectorBascula.leer() );
		registroPublico -> establecerHoraSalida( obtenerHora() );
		
		// Establece el peso leído de la etiqueta
		interfaz.establecerTextoEtiqueta( "EntradaPesoTaraPublico", to_string( registroPublico -> obtenerPesoTara() ) + " Kg" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaPublico", obtenerHora() );
		
		// Habilita la opción de poder finalizar ticket
		interfaz.establecerBotonEtiqueta( "BotonRegistrarPublico", "Finalizar" );
    }
    catch( invalid_argument &ia ){
		// Reestablece el campo de la hora y el peso de entrada
		interfaz.establecerTextoEtiqueta( "EntradaPesoTaraPublico", "No establecido" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaPublico", "No establecida" );
		
		// Muestra el error sucedido
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampoPublico", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampoPublico" );
    }
    
    // Calcula el peso neto
    publicoCalcularPesoNeto();
}

// Calcula el peso neto
void publicoCalcularPesoNeto()
{
    try{
		registroPublico -> calcularPesoNeto();
		
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoPublico", to_string( registroPublico -> obtenerPesoNeto() ) + " Kg"  );
		interfaz.establecerBotonEtiqueta( "BotonRegistrarPublico", "Finalizar" );
    }
    catch( invalid_argument &ia ){
		// No pasa nada, solo indicamos que no se estableció el peso neto e impedimos que lo registre como finalizado
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoPublico", "No establecido" );
		interfaz.establecerBotonEtiqueta( "BotonRegistrarPublico", "Pendiente" );
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

// Actualiza los registros publicos pendientes
void publicoActualizarRegistros( list< TicketPublico * > &ticketsPublicos )
{
    // Limpia el contenedor
    interfaz.removerElementosHijos( "ContenedorTickets" );

    // Itera a través de la lista de tickets pendientes y crea los tickets necesarios
    for( list< TicketPublico * >::iterator ticket = ticketsPublicos.begin(); ticket != ticketsPublicos.end(); ticket++ ){
	// Crea un elemento que será añadido a la interfaz
	Widget *elemento = new Widget();

	stringstream clave;
	clave << setfill( '0' ) << setw( 7 ) << (*ticket) -> obtenerFolio();

	try{
	    elemento -> cargarWidget( "../resources/interfaces/ElementoTicketPublico.glade" );
	    elemento -> establecerTextoEtiqueta( "ItemEntradaFolioInterno", clave.str() );
	    elemento -> establecerTextoEtiqueta( "ItemEntradaFechaInterno", (*ticket) -> obtenerFechaRegistro() );
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
