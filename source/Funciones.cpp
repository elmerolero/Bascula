#include "Funciones.h"
#include "Aplicacion.h"
#include "Registro.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>
using namespace std;

// Identificadores de señales
guint botonLectorBasculaId;
guint comboBoxLectorBasculaId;

// Banderas
bool manualActivado = false;		// Lectura manual
bool lectorBasculaActivo = false;	// La ventana para leer el peso está activado

// Ticket a crear
Ticket *ticket = nullptr;


// Registra un nuevo ticket
void registrarTicket()
{	
	// Obtiene la empresa y el producto introducidos
	string nombreEmpresa = interfaz.obtenerTextoEntrada( "EntradaNombreEmpresaInterno" );
	Registro *empresa = empresas.buscarRegistroPorNombre( nombreEmpresa );
	string nombreProducto = interfaz.obtenerTextoEntrada( "EntradaNombreProductoInterno" );
	Registro *producto = productos.buscarRegistroPorNombre( nombreProducto );
	
	try{
		// Establece los datos
		ticket -> establecerFolio( ++folioActual );
		ticket -> establecerFechaRegistro( interfaz.obtenerTextoEtiqueta( "EntradaFechaInterno" ) );
		ticket -> establecerEmpresa( empresa == nullptr ? empresas.agregarNuevoRegistro( nombreEmpresa ) : empresa );
		ticket -> establecerProducto( producto == nullptr ? productos.agregarNuevoRegistro( nombreProducto ) : producto );
		ticket -> establecerNombreConductor( interfaz.obtenerTextoEntrada( "EntradaNombreConductorInterno" ) );
		ticket -> establecerNumeroPlacas( interfaz.obtenerTextoEntrada( "EntradaNumeroPlacasInterno" ) );
		ticket -> establecerPesoBruto( interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoInterno" ) );
		ticket -> establecerHoraEntrada( interfaz.obtenerTextoEtiqueta( "EntradaHoraEntradaInterno" ) );
		ticket -> establecerDescuento( interfaz.obtenerTextoEntrada( "EntradaDescuentoInterno" ) );
		ticket -> establecerObservaciones( interfaz.obtenerTextoEntrada( "EntradaObservacionesInterno" ) );
		ticket -> establecerEntradaManual( manualActivado );
		
		// Inserta el ticket en la base de datos
		agregarTicketPendiente( ticket );
		
		//
		interfaz.establecerTextoEtiqueta( "DialogoMensaje", "Registro creado\ncorrectamente." );
		interfaz.mostrarElemento( "VentanaMensaje" );
		
		// Actualiza la vista de tickets
		actualizarTicketsPendientes();
		
		// Establece las vistas
		irHacia( nullptr, (void *)"Tickets" );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		--folioActual;
	}
}

void completarTicket( Ticket *ticket )
{
	// Obtiene los campos faltantes
	string horaSalida = interfaz.obtenerTextoEtiqueta( "EntradaHoraSalidaInterno" );
	string entradaDescuento = interfaz.obtenerTextoEntrada( "EntradaDescuentoInterno" );
	string entradaPesoNeto = interfaz.obtenerTextoEtiqueta( "EntradaPesoNetoInterno" );
	string observaciones = interfaz.obtenerTextoEntrada( "EntradaObservacionesInterno" );
	
	// Valida cada uno de ellos
	try{
		ticket -> establecerHoraSalida( horaSalida );
		ticket -> establecerObservaciones( observaciones );
		
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "Datos establecidos de forma adecuada." );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", "No se han establecido todos los datos para completar el registro." );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
	}
}

void actualizarTicketsPendientes()
{
	// Limpia el contenedor
	interfaz.removerElementosHijos( "ContenedorTickets" );
	
	// Itera a través de la lista de tickets pendientes y crea los tickets necesarios
	for( list< Ticket * >::iterator ticket = ticketsPendientes.begin(); ticket != ticketsPendientes.end(); ticket++ ){
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

// Registra el peso bruto
void registrarPesoBrutoInterno()
{
	try{
		// Establece la hora y el peso bruto en el ticket
		ticket -> establecerPesoBruto( leerBascula() );
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
	
	// Cierra el lector de la báscula
	cerrarLectorBascula();
}

// Registra el peso tara
void registrarPesoTaraInterno()
{
	try{
		// Establece el peso tara y el peso de salida
		ticket -> establecerPesoTara( leerBascula() );
		ticket -> establecerHoraSalida( obtenerHora() );
		
		// Establece el peso leído en la etiqueta
		interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", to_string( ticket -> obtenerPesoTara() ) + " Kg" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", obtenerHora() );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", "No establecido" );
		interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", "No establecida" );
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
	}
	
	// Cierra el lector de la báscula
	cerrarLectorBascula();
}

// Establece el tipo de registro (ENTRADA o SALIDA)
void seleccionarTipoRegistro( GtkWidget *widget, gpointer ptr )
{
	if( interfaz.obtenerEstadoBotonToggle( "RegistraEntrada" ) ){
		ticket -> establecerTipoRegistro( TIPO_REGISTRO_ENTRADA );
	}
	else{
		ticket -> establecerTipoRegistro( TIPO_REGISTRO_SALIDA );
	}
}

// Habilita o deshabilita el descuento
void habilitarDescuento( GtkWidget *widget, gpointer ptr )
{
	if( interfaz.obtenerEstadoBotonToggle( "SiDescuentoInterno" ) ){
		ticket -> permitirDescuento( true );
		interfaz.habilitarEdicionEntrada( "EntradaDescuentoInterno" );
	}
	else{
		ticket -> permitirDescuento( false );
		interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", "" );
		interfaz.deshabilitarEdicionEntrada( "EntradaDescuentoInterno" );
	}
}

// Cierra la ventana de mensaje
void aceptar( GtkWidget *widget, gpointer ptr )
{
	interfaz.ocultarElemento( "VentanaMensaje" );
}

// Abre el lector de la báscula
void abrirLectorBascula( GtkWidget *widget, gpointer ptr )
{
	// Muestra la ventana del lector
	interfaz.mostrarElemento( "VentanaLectorPeso" );
	
	// Establece las opciones de entrada
	establecerOpcionesBascula();
	comboBoxLectorBasculaId = interfaz.conectarSenal( "BasculasDisponibles", "changed", G_CALLBACK( iniciarOpcionBascula ), nullptr );
}

const char *leerBascula()
{
	// Establece el peso leído en la etiqueta
	return ( !manualActivado ? interfaz.obtenerTextoEtiqueta( "EtiquetaPeso" ) : interfaz.obtenerTextoEntrada( "EntradaPeso" ) );
}

// Establece las opciones para la seleccion de báscula
void establecerOpcionesBascula()
{
	// Limpia las opciones que había anteriormente
	interfaz.limpiarComboBoxText( "BasculasDisponibles" );
	
	// Agrega las opciones seleccionar y manual
	interfaz.agregarOpcionComboBoxText( "BasculasDisponibles", "Seleccionar", "Seleccionar" );
	interfaz.establecerActivoComboBoxText( "BasculasDisponibles", "Seleccionar" );
	interfaz.agregarOpcionComboBoxText( "BasculasDisponibles", "Entrada manual", NULL );
}

// Inicia la opcion seleccionada
void iniciarOpcionBascula()
{
	// Obtiene la opcion
	string opcion = interfaz.obtenerOpcionComboBoxText( "BasculasDisponibles" );
	interfaz.establecerTextoEntrada( "EntradaPeso", "" );
	interfaz.establecerTextoEtiqueta( "EtiquetaPeso", "0.0 Kg" );

	// Si la opcion seleccionada es "Introducción manual
	if( opcion.compare( "Entrada manual" ) == 0 ){
		manualActivado = true;
		interfaz.ocultarElemento( "EtiquetaPeso" );
		interfaz.mostrarElemento( "EntradaPeso" );
		interfaz.habilitarEdicionEntrada( "EntradaPeso" );
	}
	else if( opcion.compare( "Seleccionar" ) != 0 ){
		manualActivado = false;
		interfaz.deshabilitarEdicionEntrada( "EntradaPeso" );
		interfaz.ocultarElemento( "EntradaPeso" );
		interfaz.mostrarElemento( "EtiquetaPeso" );
	}
}

// Cierra el lector de la báscula
void cerrarLectorBascula()
{
	// Oculta la ventana
	interfaz.ocultarElemento( "VentanaLectorPeso" );
	
	// Desconecta la señal
	interfaz.desconectarSenal( "BotonRegistrarPeso", botonLectorBasculaId );
	interfaz.desconectarSenal( "BasculasDisponibles", comboBoxLectorBasculaId );
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
