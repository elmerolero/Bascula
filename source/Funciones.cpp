#include "Funciones.h"
#include "Aplicacion.h"
#include "Registro.h"
#include "LectorBascula.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>
#include "GestorRegistros.h"
using namespace std;

// Banderas
bool descuentoCalculado = false;

// Ticket a crear
Ticket *ticket = nullptr;

// Registra un nuevo ticket
void registrarPendienteInterno()
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
		
		crearRegistroPendiente( ticket );
		
		// Muestra el mensaje de que el ticket fue correctamente registrado
		mostrarMensaje( "Registro creado\ncorrectamente." );
		
		// Actualiza la vista de tickets
		actualizarRegistrosInternos( registrosInternosPendientes );
		
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
void finalizarPendienteInterno()
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
	
	interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Registo exitoso." );
	interfaz.mostrarElemento( "MensajeErrorCampo" );
}

// Registra el peso bruto
void registrarPesoBrutoInterno()
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
	calcularPesoNeto();
	
	// Cierra el lector de la báscula
	lectorBascula.cerrar();
}

// Registra el peso tara
void registrarPesoTaraInterno()
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
	
	leerDescuento( nullptr, nullptr );
	calcularPesoNeto();
	
	// Cierra el lector de la báscula
	lectorBascula.cerrar();
}

// Establece el tipo de registro (ENTRADA o SALIDA)
void seleccionarTipoRegistro( GtkWidget *widget, gpointer ptr )
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

// Habilita o deshabilita el descuento
void habilitarDescuento( GtkWidget *widget, gpointer ptr )
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
		calcularPesoNeto();
	}
}

// Calcula el descuento
void leerDescuento( GtkWidget *widget, gpointer ptr )
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
		calcularPesoNeto();
	}
}

void actualizarRegistrosInternos( list< Ticket * > &tickets )
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

void calcularPesoNeto()
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

// Cancela el registro
void cancelarRegistro( GtkWidget *widget, gpointer ptr )
{
	if( ticket != nullptr ){
		delete ticket;
		ticket = nullptr;
	}
	
	irHacia( nullptr, (void *)"Tickets" );
}

// Cancela la finalización
void cancelarFinalizacion( GtkWidget *widget, gpointer ptr )
{
	ticket = nullptr;
	
	irHacia( nullptr, (void *)"Tickets" );
}

// Convierte las letras minúsculas a mayúsculas
void convertirMayusculas( GtkEditable *editable, const gchar *text, gint length, gint *position, gpointer data )
{
	gchar *result = g_utf8_strup (text, length);

	g_signal_handlers_block_by_func (editable, (gpointer) convertirMayusculas, data);
	gtk_editable_insert_text (editable, result, length, position);
	g_signal_handlers_unblock_by_func (editable, (gpointer) convertirMayusculas, data);
	g_signal_stop_emission_by_name (editable, "insert_text");

	g_free (result);
}
