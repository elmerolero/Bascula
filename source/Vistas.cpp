#include "Vistas.h"
#include "Aplicacion.h"
#include "Widget.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <list>
#include "Funciones.h"
#include "LectorBascula.h"
#include "GestorRegistros.h"
using namespace std;

guint botonBasculaNuevoId = 0;
guint botonSeguimientoId = 0;
guint entradaSeguimientoId = 0;
guint botonLeerPesoBrutoId = 0;

guint botonRegistrarTicketId = 0;
guint botonCancelarInternoId = 0;

void vistaBasculaInterna( GtkWidget *widget, gpointer ptr )
{
	// Establece las vistas
	irHacia( nullptr, (void *)"Tickets" );
	
	// Obtiene los tickets pendiente
	actualizarRegistrosInternos( registrosInternosPendientes );
	
	// Desconecta las señales correspondientes
	interfaz.desconectarSenal( "BotonBasculaNuevo", botonBasculaNuevoId );
	interfaz.desconectarSenal( "EntradaSeguimiento", entradaSeguimientoId );
	interfaz.desconectarSenal( "BotonSeguimiento", botonSeguimientoId );
	
	// Conecta las señales correspondientes
    botonBasculaNuevoId = interfaz.conectarSenal( "BotonBasculaNuevo", "clicked", G_CALLBACK( vistaCrearRegistro ), nullptr );
    entradaSeguimientoId = interfaz.conectarSenal( "EntradaSeguimiento", "activate", G_CALLBACK( vistaFinalizarRegistro ), nullptr );
    botonSeguimientoId = interfaz.conectarSenal( "BotonSeguimiento", "clicked", G_CALLBACK( vistaFinalizarRegistro ), nullptr );
}

void vistaCrearRegistro( GtkWidget *widget, gpointer ptr )
{	
	// Oculta los errores que pudieron haber ocurrido en registros anteriores
	interfaz.ocultarElemento( "MensajeErrorCampo" );
	
	// Folio
	stringstream folio;
	folio << setfill( '0' ) << setw( 7 ) << (folioActual + 1);
	interfaz.establecerTextoEtiqueta( "EntradaFolioInterno", folio.str() );
	
	// Fecha
	interfaz.establecerTextoEtiqueta( "EntradaFechaInterno", obtenerFecha() );
	
	// Empresa
	interfaz.habilitarEdicionEntrada( "EntradaNombreEmpresaInterno" );
	interfaz.establecerTextoEntrada( "EntradaNombreEmpresaInterno", "" );
	
	// Producto
	interfaz.habilitarEdicionEntrada( "EntradaNombreProductoInterno" );
	interfaz.establecerTextoEntrada( "EntradaNombreProductoInterno", "" );
	
	// Nombre del conductor
	interfaz.habilitarEdicionEntrada( "EntradaNombreConductorInterno" );
	interfaz.establecerTextoEntrada( "EntradaNombreConductorInterno", "" );
	
	// Numero de placas
	interfaz.habilitarEdicionEntrada( "EntradaNumeroPlacasInterno" );
	interfaz.establecerTextoEntrada( "EntradaNumeroPlacasInterno", "" );
	
	// Hora entrada y Peso Bruto
	interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaInterno", "No establecida" );
	interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoInterno", "No establecido" );
	
	// Hora salida y Peso Tara
	interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", "No establecida" );
	interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", "No establecido" );
	
	// Descuento
	interfaz.establecerActivoBotonToggle( "NoDescuentoInterno" );
	interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", "" );
	interfaz.deshabilitarEdicionEntrada( "EntradaDescuentoInterno" );
	
	// Tipo de registro
	interfaz.establecerActivoBotonToggle( "RegistraEntrada" );
	
	// Peso neto
	interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", "No establecido" );
	
	// Observaciones
	interfaz.establecerTextoEntrada( "EntradaObservacionesInterno", "" );
	
	
	// Establece la señal para leer peso bruto
	interfaz.desconectarSenal( "BotonLeerPesoBrutoInterno", botonLeerPesoBrutoId );
	botonLeerPesoBrutoId = interfaz.conectarSenal( "BotonLeerPesoBrutoInterno", "clicked", G_CALLBACK( vistaLeerPesoBruto ), nullptr );
	
	// Establece la señal para registrar el ticket
	interfaz.desconectarSenal( "BotonRegistrarInterno", botonRegistrarTicketId );
	botonRegistrarTicketId = interfaz.conectarSenal( "BotonRegistrarInterno", "clicked", G_CALLBACK( registrarPendienteInterno ), nullptr );
	
	// Establece la señal para cancelar el registro
	interfaz.desconectarSenal( "EnlaceRegresarInterno", botonCancelarInternoId );
	botonCancelarInternoId = interfaz.conectarSenal( "EnlaceRegresarInterno", "activate-link", G_CALLBACK( cancelarRegistro ), nullptr );
	
	// Crea el nuevo ticket
	ticket = new Ticket();
	
	// Si el ticket fue creado correctamente
	if( ticket != nullptr ){
		// Establece la vista de nuevo ticket
		irHacia( nullptr, (void *)"NuevoTicketInterno" );
	}
	else{
		// Muestra el mensaje de que el ticket no fue creado
		interfaz.establecerTextoEtiqueta( "DialogoMensaje", "No se ha podido crear un\nnuevo ticket." );
		interfaz.mostrarElemento( "VentanaMensaje" );
	}
}

void vistaLeerPesoBruto()
{
	interfaz.ocultarElemento( "MensajeErrorCampo" );
	lectorBascula.abrir( interfaz );
	lectorBascula.establecerIdSenal( interfaz.conectarSenal( "BotonRegistrarPeso", "clicked", G_CALLBACK( registrarPesoBrutoInterno ), nullptr ) );
}

void vistaLeerPesoTara()
{
	string pesoBruto = interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoInterno" );
	if( !ticket -> estaPesoBrutoEstablecido() ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Es necesario registrar primero el peso bruto." );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		return;
	}
	interfaz.ocultarElemento( "MensajeErrorCampo" );
	lectorBascula.abrir( interfaz );
	lectorBascula.establecerIdSenal( interfaz.conectarSenal( "BotonRegistrarPeso", "clicked", G_CALLBACK( registrarPesoTaraInterno ), nullptr ) );
}

void vistaFinalizarRegistro()
{
	// Busca el ticket 
	ticket = buscarRegistroInternoPorNumeroPlaca( interfaz.obtenerTextoEntrada( "EntradaSeguimiento" ), registrosInternosPendientes );
	
	// Si no encuentra el ticket
	if( ticket == nullptr ){
		interfaz.establecerTextoEtiqueta( "DialogoMensaje", "Registro no encontrado." );
		interfaz.mostrarElemento( "VentanaMensaje" );
		return;
	}
	
	// Oculta los errores que pudieron haber ocurrido en registros anteriores
	interfaz.ocultarElemento( "MensajeErrorCampo" );
	
	// Establece los datos de la vista
	stringstream entrada;
	
	// Folio
	entrada << setfill( '0' ) << setw( 7 ) << ticket -> obtenerFolio();
	interfaz.establecerTextoEtiqueta( "EntradaFolioInterno", entrada.str() );
	
	// Fecha
	interfaz.establecerTextoEtiqueta( "EntradaFechaInterno", ticket -> obtenerFechaRegistro() );
	
	// Tipo registro
	if( ticket -> obtenerTipoRegistro() == TIPO_REGISTRO_ENTRADA ){
		interfaz.establecerActivoBotonToggle( "RegistraEntrada" );
	}
	else{
		interfaz.establecerActivoBotonToggle( "RegistraSalida" );
	}

	// Nombre de la empresa (no permite edicion)
	interfaz.establecerTextoEntrada( "EntradaNombreEmpresaInterno", ticket -> obtenerEmpresa() -> obtenerNombre() );
	interfaz.deshabilitarEdicionEntrada( "EntradaNombreEmpresaInterno" );
	
	// Nombre del producto (no permite edicion)
	interfaz.establecerTextoEntrada( "EntradaNombreProductoInterno", ticket -> obtenerProducto() -> obtenerNombre() );
	interfaz.deshabilitarEdicionEntrada( "EntradaNombreProductoInterno" );
	
	// Nombre del conductor (no permite edicion)
	interfaz.establecerTextoEntrada( "EntradaNombreConductorInterno", ticket -> obtenerNombreConductor() );
	interfaz.deshabilitarEdicionEntrada( "EntradaNombreConductorInterno" );
	
	// Numero de placas (no permite edicion)
	interfaz.establecerTextoEntrada( "EntradaNumeroPlacasInterno", ticket -> obtenerNumeroPlacas() );
	interfaz.deshabilitarEdicionEntrada( "EntradaNumeroPlacasInterno" );
	
	// Hora entrada y peso bruto
	interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaInterno", ( ticket -> estaPesoBrutoEstablecido() ? ticket -> obtenerHoraEntrada() : "No establecida" ) );
	interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoInterno", ( ticket -> estaPesoBrutoEstablecido() ? to_string( ticket -> obtenerPesoBruto() ) + " Kg" : "No establecido" ) );
	if( ticket -> estaPesoBrutoEstablecido() ){
		// Deshabilita la señal para leer el peso bruto
		interfaz.desconectarSenal( "BotonLeerPesoBrutoInterno", botonLeerPesoBrutoId );
	}
	
	// Hora salida y peso tara
	interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", ( ticket -> estaPesoTaraEstablecido() ? ticket -> obtenerHoraSalida() : "No establecida" ) );
	interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", ( ticket -> estaPesoTaraEstablecido() ? to_string( ticket -> obtenerPesoTara() ) + " Kg" : "No establecido" ) );
	
	// Descuento
	if( ticket -> permitirDescuento() ){
		interfaz.establecerActivoBotonToggle( "SiDescuentoInterno" );
		interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", to_string( ticket -> obtenerDescuento() ) );
	}
	else{
		interfaz.establecerActivoBotonToggle( "NoDescuentoInterno" );
		interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", "" );
	}
	
	// Peso neto
	interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", ( ticket -> estaPesoNetoCalculado() ? to_string( ticket -> obtenerPesoNeto() ) + " Kg" : "No establecido" ) );
	
	// Motivos y observaciones
	interfaz.establecerTextoEntrada( "EntradaObservacionesInterno", ticket -> obtenerObservaciones() );
	
	// Establece la señal para cancelar el registro
	interfaz.desconectarSenal( "EnlaceRegresarInterno", botonCancelarInternoId );
	botonCancelarInternoId = interfaz.conectarSenal( "EnlaceRegresarInterno", "activate-link", G_CALLBACK( cancelarFinalizacion ), nullptr );
	
	// Establece la señal para registrar el ticket
	interfaz.desconectarSenal( "BotonRegistrarInterno", botonRegistrarTicketId );
	botonRegistrarTicketId = interfaz.conectarSenal( "BotonRegistrarInterno", "clicked", G_CALLBACK( finalizarPendienteInterno ), nullptr );
	
	// 
	interfaz.establecerBotonEtiqueta( "BotonRegistrarInterno", "Finalizar" );

	// Va hacia la vista
	irHacia( nullptr, (void *)"NuevoTicketInterno" );
}
