#include "Vistas.h"
#include "Aplicacion.h"
#include "Widget.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <list>
#include "Funciones.h"
using namespace std;

guint botonBasculaNuevoId = 0;
guint botonSeguimientoId = 0;
guint entradaSeguimientoId = 0;
guint botonLeerPesoBrutoId = 0;

void vistaBasculaInterna( GtkWidget *widget, gpointer ptr )
{
	// Establece las vistas
	irHacia( nullptr, (void *)"Tickets" );
	
	// Obtiene los tickets pendiente
	actualizarTicketsPendientes();
	
	// Desconecta las señales correspondientes
	if( botonBasculaNuevoId > 0 ){
		interfaz.desconectarSenal( "BotonBasculaNuevo", botonBasculaNuevoId );
		interfaz.desconectarSenal( "EntradaSeguimiento", entradaSeguimientoId );
		interfaz.desconectarSenal( "BotonSeguimiento", botonSeguimientoId );
	}
	
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
	if( botonLeerPesoBrutoId > 0 ){
		interfaz.desconectarSenal( "BotonLeerPesoBrutoInterno", botonLeerPesoBrutoId );
		botonLeerPesoBrutoId = 0;
	}
	botonLeerPesoBrutoId = interfaz.conectarSenal( "BotonLeerPesoBrutoInterno", "clicked", G_CALLBACK( abrirLectorBascula ), nullptr );
	
	// Establece la vista de nuevo ticket
	irHacia( nullptr, (void *)"NuevoTicketInterno" );
	
	// Crea el nuevo ticket
	ticket = new Ticket();
}

void vistaLeerPesoBruto()
{
	// Conecta la señal
	botonLectorBasculaId = interfaz.conectarSenal( "BotonRegistrarPeso", "clicked", G_CALLBACK( registrarPesoBrutoInterno ), nullptr );
	abrirLectorBascula( nullptr, nullptr );
}

void vistaFinalizarRegistro()
{
	// Busca el ticket 
	ticket = buscarTicketPorNumeroPlaca( interfaz.obtenerTextoEntrada( "EntradaSeguimiento" ) );
	
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
	interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaInterno", ticket -> obtenerHoraEntrada() );
	interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoInterno", to_string( ticket -> obtenerPesoBruto() ) );
	
	// Hora salida y peso tara
	interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", "No establecido" );
	interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", "No establecido" );
	
	// Tipo registro
	if( ticket -> obtenerTipoRegistro() == TIPO_REGISTRO_ENTRADA ){
		interfaz.establecerActivoBotonToggle( "RegistraEntrada" );
	}
	else{
		interfaz.establecerActivoBotonToggle( "RegistraSalida" );
	}
	
	// Descuento
	if( ticket -> obtenerDescuento() > 0 ){
		interfaz.establecerActivoBotonToggle( "SiDescuentoInterno" );
		interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", to_string(ticket -> obtenerDescuento() ) );
	}
	
	// Peso neto
	interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", "No establecido" );
	
	// Motivos y observaciones
	interfaz.establecerTextoEntrada( "EntradaObservacionesInterno", ticket -> obtenerObservaciones() );
	
	// Deshabilita la señal para leer el peso bruto
 	if( botonLeerPesoBrutoId > 0 ){
		interfaz.desconectarSenal( "BotonLeerPesoBrutoInterno", botonLeerPesoBrutoId );
		botonLeerPesoBrutoId = 0;
	}

	// Va hacia la vista
	irHacia( nullptr, (void *)"NuevoTicketInterno" );
}
