#include "Vistas.h"
#include "Aplicacion.h"
#include "Widget.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <list>
#include "Funciones.h"
#include "Sesion.h"
#include "LectorBascula.h"
#include "GestorBasculas.h"
#include "GestorRegistros.h"
#include "RegistrosInternos.h"
#include "RegistrosPublicos.h"
#include "Senales.h"
using namespace std;

void vistaCuenta( GtkWidget *widget, gpointer ptr )
{
	// Establece las opciones para cuenta	
	interfaz.establecerTextoEntrada( "EntradaCuentaNombre", usuario.obtenerNombre() );
	interfaz.establecerTextoEntrada( "EntradaCuentaApellidos", usuario.obtenerApellidos() );
	interfaz.establecerTextoEntrada( "EntradaCuentaNombreUsuario", usuario.obtenerNombreUsuario() );
	interfaz.establecerTextoEntrada( "EntradaCuentaContrasenaNueva", "" );
	interfaz.establecerTextoEntrada( "EntradaCuentaContrasenaConfirmacion", "" );

	// Se dirige a cuenta
	irA( "Cuenta", false );
}

void vistaRegistrosEmpresas( GtkWidget *widget, gpointer ptr )
{	
	// Establece la vista para consultar registros
	irA( "ConsultarRegistros", false );

	// Establece el nombre de los registros que se estan consultando
	interfaz.establecerTextoEtiqueta( "Titulo", "Empresas" );
	
	// Establece la imagen de registro
	interfaz.establecerImagen( "ImagenRegistro", "../resources/images/icons/Empresas.png" );
	interfaz.establecerImagen( "ImagenEdicionRegistro", "../resources/images/icons/Producto.png" );
	
	// Actualiza la lista de registros de empresas
	empresas.actualizarListaRegistros();
	
	// Vistas
	interfaz.conectarSenal( botonRegistroVistaConsultar, G_CALLBACK( vistaRegistro ), (void *)(&empresas) );
	interfaz.conectarSenal( botonRegistroVistaNuevo, G_CALLBACK( irHacia ), (void *)"NuevoRegistro" );
	interfaz.conectarSenal( botonRegistroVistaEditar, G_CALLBACK( vistaRegistroEditar ), (void *)(&empresas) );
	interfaz.conectarSenal( botonRegistroVistaEliminar, G_CALLBACK( vistaRegistroEliminar ), nullptr );

	// Acciones
	interfaz.conectarSenal( botonRegistroGuardarNuevo, G_CALLBACK( crearRegistro ), (void *)(&empresas) );
    interfaz.conectarSenal( botonRegistroGuardarEdicion, G_CALLBACK( actualizarRegistro ), (void *)(&empresas) );
    interfaz.conectarSenal( botonSi, G_CALLBACK( eliminarRegistro ), (void *)(&empresas) );
}

void vistaRegistrosProductos( GtkWidget *widget, gpointer ptr )
{
	// Establece la vista de consultar registros
	irHacia( nullptr, (void *)"ConsultarRegistros" );
	
	// Establece el nombre de los registros que se estan consultando
	interfaz.establecerTextoEtiqueta( "Titulo", "Productos" );
	
	// Establece la imagen de registro
	interfaz.establecerImagen( "ImagenRegistro", "../resources/images/icons/Producto.png" );
	interfaz.establecerImagen( "ImagenEdicionRegistro", "../resources/images/icons/Producto.png" );
	
	// Actualiza la lista de registros de empresas
	productos.actualizarListaRegistros();
	
	// Conecta las señales de la vista
	interfaz.conectarSenal( botonRegistroVistaConsultar, G_CALLBACK( vistaRegistro ), (void *)(&productos) );
	interfaz.conectarSenal( botonRegistroVistaNuevo, G_CALLBACK( irHacia ), (void *)"NuevoRegistro" );
	interfaz.conectarSenal( botonRegistroVistaEditar, G_CALLBACK( vistaRegistroEditar ), (void *)(&productos) );
	interfaz.conectarSenal( botonRegistroVistaEliminar, G_CALLBACK( vistaRegistroEliminar ), nullptr );

	interfaz.conectarSenal( botonRegistroGuardarNuevo, G_CALLBACK( crearRegistro ), (void *)(&productos) );
    interfaz.conectarSenal( botonRegistroGuardarEdicion, G_CALLBACK( actualizarRegistro ), (void *)(&productos) );
	interfaz.conectarSenal( botonSi, G_CALLBACK( eliminarRegistro ), (void *)(&productos) );
}

void vistaRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data )
{
	// Convierte el apuntador tipo void en un apuntado tipo ContenedorRegistros
	ContenedorRegistros *contenedor = static_cast< ContenedorRegistros * >( data );
	
	// Obtiene el nombre del registro a consultar
	Registro *registro = contenedor -> buscarRegistroPorClave( obtenerFolioSelector( row ) );
	if( registro == nullptr ){
		mostrarMensaje( "Registro no encontrado." );
		return;
	}

	// Establece la clave del registro
	stringstream clave;
	clave << setfill( '0' ) << setw( 7 ) << ( registro -> obtenerClave() );
	interfaz.establecerTextoEtiqueta( "EtiquetaClaveRegistro", clave.str() );
	
	// Establece el nombre del registro
	interfaz.establecerTextoEtiqueta( "EtiquetaNombreRegistro", registro -> obtenerNombre() );

	// Establece la vista de registro
	irA( "Registro", false );
}

void vistaRegistroEditar( GtkWidget *widget, gpointer data )
{
	try{
		// Convierte el apuntador de tipo void a un apuntador de tipo ContenedorRegistros
		ContenedorRegistros *contenedor = static_cast< ContenedorRegistros * >( data );

		// Obtiene la clave del widget seleccionado
		unsigned int clave = stoi( interfaz.obtenerWidgetSeleccionadoListBox( "ContenedorRegistros" ) );

		// Recupera el registro por si clave
		Registro *registro = contenedor -> buscarRegistroPorClave( clave );
		if( registro == nullptr ){
			throw runtime_error( "Ocurrió un problema al recuperar el registro seleccionado." );
		}

		// Estabece los datos del formulario
		interfaz.establecerTextoEtiqueta( "EtiquetaClaveEditarRegistro", to_string( registro -> obtenerClave() ) );
		interfaz.establecerTextoEntrada( "EntradaNombreEditarRegistro", registro -> obtenerNombre() );

		irA( "EditarRegistro", false );
	}
	catch( invalid_argument &ia ){
		mostrarMensajeError( ia.what() );
	}
}

void vistaRegistroEliminar( GtkWidget *widget, gpointer ptr )
{
	interfaz.establecerTextoEtiqueta( "MensajeAlerta", "ALERTA: Si el existen registros de pesaje que hagan\nreferencia a este registro, también serán eliminados.\n¿Estás seguro que deseas hacerlo?" );
	interfaz.mostrarElemento( "VentanaSiNo" );
}

void vistaBasculaEliminar()
{
	interfaz.establecerTextoEtiqueta( "MensajeAlerta", "¿Quieres eliminar la báscula seleccionada?" );
	interfaz.mostrarElemento( "VentanaSiNo" );
}

void vistaBasculaEdicion()
{
	basculaLimpiarFormulario();

	irA( "OpcionesBascula", false );
}


void vistaConfiguracion( GtkWidget *widget, gpointer ptr )
{
	// Numero copias
	interfaz.establecerTextoEntrada( "OpcionesImpresionFormatos", to_string( numeroFormatos ) );
	interfaz.establecerTextoEntrada( "OpcionesImpresionCopias", to_string( numeroCopias ) );

	// Establece la señal de eliminar bascula
	interfaz.conectarSenal( botonSi, G_CALLBACK( basculaEliminar ), nullptr );

	// Se dirige a la vista
	irA( "Configuracion", false );
}

void vistaBasculaPublica( GtkWidget *widget, gpointer ptr )
{	
	// Obtiene los tickets pendientes
	publicoActualizarRegistros( registrosPublicosPendientes, "ContenedorTickets" );
	
	// Conecta las señales correspondientes
    interfaz.conectarSenal( botonBasculaNuevo, G_CALLBACK( vistaCrearRegistroPublico ), nullptr );
	interfaz.conectarSenal( ticketsPendientesSeleccionados, G_CALLBACK( vistaPublicoEditarRegistro ), nullptr );

	// Establece las vistas
	irA( "Tickets", false );
}

void vistaBasculaInterna( GtkWidget *widget, gpointer ptr )
{	
	// Obtiene los tickets pendiente
	internoActualizarRegistros( registrosInternosPendientes, "ContenedorTickets" );
	
	// Conecta las señales correspondientes
    interfaz.conectarSenal( botonBasculaNuevo, G_CALLBACK( vistaCrearRegistro ), nullptr );
    interfaz.conectarSenal( ticketsPendientesSeleccionados, G_CALLBACK( vistaInternoEditarRegistro ), nullptr);

    // Establece las vistas
	irHacia( nullptr, (void *)"Tickets" );
}

void vistaCrearRegistroPublico( GtkWidget *widget, gpointer ptr )
{	
	// Folio
	stringstream folio;
	folio << setfill( '0' ) << setw( 7 ) << (folioActualPublico + 1);
	interfaz.establecerTextoEtiqueta( "EntradaFolioPublico", folio.str() );
	
	// Fecha
	interfaz.establecerTextoEtiqueta( "EntradaFechaPublico", obtenerFecha() );
	
	// Producto
	interfaz.habilitarEdicionEntrada( "EntradaNombreProductoPublico" );
	interfaz.establecerTextoEntrada( "EntradaNombreProductoPublico", "" );
	
	// Nombre del conductor
	interfaz.habilitarEdicionEntrada( "EntradaNombreConductorPublico" );
	interfaz.establecerTextoEntrada( "EntradaNombreConductorPublico", "" );
	
	// Numero de placas
	interfaz.habilitarEdicionEntrada( "EntradaNumeroPlacasPublico" );
	interfaz.establecerTextoEntrada( "EntradaNumeroPlacasPublico", "" );
	
	// Hora entrada y Peso Bruto
	interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaPublico", "No establecida" );
	interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoPublico", "No establecido" );
	
	// Hora salida y Peso Tara
	interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaPublico", "No establecida" );
	interfaz.establecerTextoEtiqueta( "EntradaPesoTaraPublico", "No establecido" );
	
	// Tipo de registro
	interfaz.establecerActivoBotonToggle( "ViajeLocal" );
	
	// Peso neto
	interfaz.establecerTextoEtiqueta( "EntradaPesoNetoPublico", "No establecido" );
	
	// Establece el completador de producto
	interfaz.establecerCompletadorEntrada( "EntradaNombreProductoInterno", NULL );
	interfaz.establecerCompletadorEntrada( "EntradaNombreProductoPublico", productos.obtenerCompletador() ); 
	
	// Establece la vista de nuevo ticket
	irA( "NuevoTicketPublico", false );
}

void vistaPublicoEditarRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data )
{
	try{
		// Busca el ticket
		TicketPublico *registroPublico = buscarRegistroPublicoPorFolio( obtenerFolioSelector( row ), registrosPublicosPendientes );
		if( registroPublico == nullptr ){
			interfaz.establecerTextoEtiqueta( "DialogoMensaje", "Registro no encontrado." );
			interfaz.mostrarElemento( "VentanaMensaje" );
			return;
		}
		
		// Establece los datos de la vista
		stringstream entrada;
		
		// Folio
		entrada << setfill( '0' ) << setw( 7 ) << registroPublico -> obtenerFolio();
		interfaz.establecerTextoEtiqueta( "EntradaFolioPublico", entrada.str() );
		
		// Fecha
		interfaz.establecerTextoEtiqueta( "EntradaFechaPublico", registroPublico -> obtenerFecha() );
		
		// Tipo registro
		if( registroPublico -> obtenerTipoViaje() == VIAJE_LOCAL ){
			interfaz.establecerActivoBotonToggle( "ViajeLocal" );
		}
		else{
			interfaz.establecerActivoBotonToggle( "ViajeForaneo" );
		}
		
		// Nombre del producto (no permite edicion)
		interfaz.establecerTextoEntrada( "EntradaNombreProductoPublico", registroPublico -> obtenerProducto() -> obtenerNombre() );
		interfaz.deshabilitarEdicionEntrada( "EntradaNombreProductoPublico" );
		
		// Nombre del conductor (no permite edicion)
		interfaz.establecerTextoEntrada( "EntradaNombreConductorPublico", registroPublico -> obtenerNombreConductor() );
		interfaz.deshabilitarEdicionEntrada( "EntradaNombreConductorPublico" );
		
		// Numero de placas (no permite edicion)
		interfaz.establecerTextoEntrada( "EntradaNumeroPlacasPublico", registroPublico -> obtenerNumeroPlacas() );
		interfaz.deshabilitarEdicionEntrada( "EntradaNumeroPlacasPublico" );
		
		// Hora entrada y peso bruto
		interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaPublico", ( registroPublico -> estaPesoBrutoEstablecido() ? registroPublico -> obtenerHoraEntrada() : "No establecida" ) );
		interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoPublico", ( registroPublico -> estaPesoBrutoEstablecido() ? pesoString( registroPublico -> obtenerPesoBruto(), 2 ) : "No establecido" ) );
		
		// Hora salida y peso tara
		interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaPublico", ( registroPublico -> estaPesoTaraEstablecido() ? registroPublico -> obtenerHoraSalida() : "No establecida" ) );
		interfaz.establecerTextoEtiqueta( "EntradaPesoTaraPublico", ( registroPublico -> estaPesoTaraEstablecido() ? pesoString( registroPublico -> obtenerPesoTara(), 2 ) : "No establecido" ) );
		
		// Peso neto
		interfaz.establecerTextoEtiqueta( "EntradaPesoNetoPublico", ( registroPublico -> estaPesoNetoEstablecido() ? pesoString( registroPublico -> obtenerPesoNeto(), 2 ) : "No establecido" ) );
		
		// Establece el texto de finalizar
		interfaz.establecerBotonEtiqueta( "BotonRegistrarPublico", "Finalizar" );

		// Va hacia la vista
		irA( "NuevoTicketPublico", false );
	}
	catch( invalid_argument &ia ){
		mostrarMensaje( "Introduzca un folio válido." );
	} 
}

void vistaCrearRegistro( GtkWidget *widget, gpointer ptr )
{	
	// Folio
	stringstream folio;
	folio << setfill( '0' ) << setw( 7 ) << (folioActual + 1);
	interfaz.establecerTextoEtiqueta( "EntradaFolioInterno", folio.str() );
	
	// Fecha
	interfaz.establecerTextoEtiqueta( "EntradaFechaInterno", obtenerFecha() );
	
	// Limpia el formulario
	internoLimpiarFormulario();

	// Descuento
	interfaz.establecerActivoBotonToggle( "NoDescuentoInterno" );
	interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", "" );
	interfaz.deshabilitarEdicionEntrada( "EntradaDescuentoInterno" );

	// Tipo de registro (registra entrada por defecto)
	interfaz.establecerActivoBotonToggle( "RegistraEntrada" );
	
	// Establece el completador de empresa
	interfaz.establecerCompletadorEntrada( "EntradaNombreEmpresaInterno", empresas.obtenerCompletador() );
	
	// Establece el completador de producto
	interfaz.establecerCompletadorEntrada( "EntradaNombreProductoPublico", NULL );
	interfaz.establecerCompletadorEntrada( "EntradaNombreProductoInterno", productos.obtenerCompletador() );
	
	// Establece la vista de nuevo ticket 
	irHacia( nullptr, (void *)"NuevoTicketInterno" );
}

void vistaInternoEditarRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data )
{
	Ticket *ticket = buscarRegistroInternoPorFolio( obtenerFolioSelector( row ), registrosInternosPendientes );
	
	// Si no encuentra el ticket
	if( ticket == nullptr ){
		interfaz.establecerTextoEtiqueta( "DialogoMensaje", "Registro no encontrado." );
		interfaz.mostrarElemento( "VentanaMensaje" );
		return;
	}
	
	// Establece los datos de la vista
	stringstream entrada;
	
	// Folio
	entrada << setfill( '0' ) << setw( 7 ) << ticket -> obtenerFolio();
	interfaz.establecerTextoEtiqueta( "EntradaFolioInterno", entrada.str() );
	
	// Fecha
	interfaz.establecerTextoEtiqueta( "EntradaFechaInterno", ticket -> obtenerFecha() );
	
	// Tipo registro
	if( ticket -> obtenerTipoRegistro() == TIPO_REGISTRO_ENTRADA ){
		interfaz.establecerActivoBotonToggle( "RegistraEntrada" );
		interfaz.establecerTextoEtiqueta( "EtiquetaEmpresa", "Proveedor:      " );
	}
	else{
		interfaz.establecerActivoBotonToggle( "RegistraSalida" );
		interfaz.establecerTextoEtiqueta( "EtiquetaEmpresa", "Cliente:           " );
	}

	// Nombre de la empresa
	interfaz.establecerTextoEntrada( "EntradaNombreEmpresaInterno", ticket -> obtenerEmpresa() -> obtenerNombre() );
	
	// Nombre del producto
	interfaz.establecerTextoEntrada( "EntradaNombreProductoInterno", ticket -> obtenerProducto() -> obtenerNombre() );
	
	// Nombre del conductor
	interfaz.establecerTextoEntrada( "EntradaNombreConductorInterno", ticket -> obtenerNombreConductor() );
	
	// Numero de placas
	interfaz.establecerTextoEntrada( "EntradaNumeroPlacasInterno", ticket -> obtenerNumeroPlacas() );
	
	// Hora entrada y peso bruto
	interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaInterno", ( ticket -> estaPesoBrutoEstablecido() ? ticket -> obtenerHoraEntrada() : "No establecida" ) );
	interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoInterno", ( ticket -> estaPesoBrutoEstablecido() ? pesoString( ticket -> obtenerPesoBruto(), 2 ) : "No establecido" ) );
	
	// Hora salida y peso tara
	interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", ( ticket -> estaPesoTaraEstablecido() ? ticket -> obtenerHoraSalida() : "No establecida" ) );
	interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", ( ticket -> estaPesoTaraEstablecido() ? pesoString( ticket -> obtenerPesoTara(), 2 ) : "No establecido" ) );
	
	// Descuento
	if( ticket -> permitirDescuento() ){
		interfaz.establecerActivoBotonToggle( "SiDescuentoInterno" );
		interfaz.habilitarEdicionEntrada( "EntradaDescuentoInterno" );
		interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", pesoString( ticket -> obtenerDescuento(), 2, false ) );
	}
	else{
		interfaz.establecerActivoBotonToggle( "NoDescuentoInterno" );
		interfaz.deshabilitarEdicionEntrada( "EntradaDescuentoInterno" );
		interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", "" );
	}

	// Peso neto
	interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", ( ticket -> estaPesoNetoEstablecido() ? pesoString( ticket -> obtenerPesoNeto(), 2 ) : "No establecido" ) );
		
	// Motivos y observaciones
	interfaz.establecerTextoEntrada( "EntradaObservacionesInterno", ticket -> obtenerObservaciones() );
	
	// Va hacia la vista
	irHacia( nullptr, (void *)"NuevoTicketInterno" );
}

void vistaLeerPesoBruto()
{
	basculaAbrirLector();
	interfaz.conectarSenal( botonRegistrarPeso, G_CALLBACK( internoRegistrarPesoBruto ), nullptr );
}

void vistaLeerPesoTara()
{
	basculaAbrirLector();
	interfaz.conectarSenal( botonRegistrarPeso, G_CALLBACK( internoRegistrarPesoTara ), nullptr );
}

void vistaLeerPesoBrutoPublico()
{
	basculaAbrirLector();
	interfaz.conectarSenal( botonRegistrarPeso, G_CALLBACK( publicoRegistrarPesoBruto ), nullptr );
}

void vistaLeerPesoTaraPublico()
{
	basculaAbrirLector();
	interfaz.conectarSenal( botonRegistrarPeso, G_CALLBACK( publicoRegistrarPesoTara ), nullptr );
}

void vistaConsultarPesajesInternos()
{
	// Muestra los registros del día
	internoObtenerPorFecha( registrosInternosConsultados, obtenerFecha() );
	internoActualizarRegistros( registrosInternosConsultados, "ContenedorRegistrosPesaje" );

	// Establece la fecha del ticket que se está consultando
	interfaz.establecerTextoEtiqueta( "TicketsRegistrados", "Registros del día " + obtenerFecha() + ":"  );

	// Establece el número de tickets
	interfaz.establecerTextoEtiqueta( "TicketsContados", to_string( registrosInternosConsultados.size() ) + " registros"  );;

	// Señal del boton seleccionar día
	interfaz.conectarSenal( botonRegistrosPesajeSeleccionarDia, G_CALLBACK( irHacia ), ( void *)"ConsultarDia" );

	// Señal del boton informe
	interfaz.conectarSenal( botonRegistrosPesajeObtenerInforme, G_CALLBACK( internoGenerarInforme ), nullptr );

	// Señal de obtener registros
	interfaz.conectarSenal( botonObtenerRegistros, G_CALLBACK( irHacia ), (void *)"ObtenerRegistros" );

	// Señal de selección de ticket
	interfaz.conectarSenal( ticketsConsultadosSeleccionados, G_CALLBACK( vistaInternoConsultarRegistro ), nullptr );

	// Boton de seleccion dia
	interfaz.conectarSenal( botonConsultarDia, G_CALLBACK( internoSeleccionarDia ), nullptr );

	// Boton de generación de informe
	interfaz.conectarSenal( botonObtenerRegistrosRango, G_CALLBACK( internoObtenerRegistrosRango ), nullptr );

	// Redirige hacia la vista
	irHacia( nullptr, (void *)"Pesajes" );
}

void vistaConsultarPesajesPublicos()
{
	// Muestra los registros del día
	publicoObtenerPorFecha( registrosPublicosConsultados, obtenerFecha() );
	publicoActualizarRegistros( registrosPublicosConsultados, "ContenedorRegistrosPesaje" );

	// Establece la fecha del ticket que se está consultando
	interfaz.establecerTextoEtiqueta( "TicketsRegistrados", "Registros del día " + obtenerFecha() + ":"  );

	// Establece el número de tickets
	interfaz.establecerTextoEtiqueta( "TicketsContados", to_string( registrosPublicosConsultados.size() ) + " registros"  );

	// Señal del boton seleccionar día
	interfaz.conectarSenal( botonRegistrosPesajeSeleccionarDia, G_CALLBACK( irHacia ), ( void *)"ConsultarDia" );

	// Señal del boton informe
	interfaz.conectarSenal( botonRegistrosPesajeObtenerInforme, G_CALLBACK( publicoGenerarInforme ), nullptr );

	// Señal de obtener registros
	interfaz.conectarSenal( botonObtenerRegistros, G_CALLBACK( irHacia ), (void *)"ObtenerRegistros" );	

	// Señal de selección de ticket
	interfaz.conectarSenal( ticketsConsultadosSeleccionados, G_CALLBACK( vistaPublicoConsultarRegistro ), nullptr );

	// Boton de seleccion dia
	interfaz.conectarSenal( botonConsultarDia, G_CALLBACK( publicoSeleccionarDia ), nullptr );

	// Boton de generación de informe
	interfaz.conectarSenal( botonObtenerRegistrosRango, G_CALLBACK( publicoObtenerRegistrosRango ), nullptr );

	// Redirige hacia la vista
	irHacia( nullptr, (void *)"Pesajes" );
}

void vistaInternoConsultarRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data )
{
	// Obtiene el ticket que se desea consultar
	Ticket *ticket = buscarRegistroInternoPorFolio( obtenerFolioSelector( row ), registrosInternosConsultados );
	if( ticket == nullptr ){
		mostrarMensaje( "Registro no encontrado." );
		return;
	}

	// Establece los datos del ticket en el formulario
	interfaz.establecerTextoEtiqueta( "FolioInterno", to_string( ticket -> obtenerFolio() ) ); 					// Folio
	interfaz.establecerTextoEtiqueta( "FechaInterno", ticket -> obtenerFecha() );			   					// Fecha
	interfaz.establecerTextoEntrada( "NombreEmpresaInterno", ticket -> obtenerEmpresa() -> obtenerNombre() );	// Empresa
	interfaz.establecerTextoEntrada( "NombreProductoInterno", ticket -> obtenerProducto() -> obtenerNombre() );	// Producto
	interfaz.establecerTextoEntrada( "NombreConductorInterno", ticket -> obtenerNombreConductor() );			// Nombre del conductor
	interfaz.establecerTextoEntrada( "NumeroPlacasInterno", ticket -> obtenerNumeroPlacas() );					// Número de placas
	interfaz.establecerTextoEtiqueta( "HoraEntradaInterno", ticket -> obtenerHoraEntrada() );					// Hora entrada
	interfaz.establecerTextoEtiqueta( "PesoBrutoInterno", to_string( ticket -> obtenerPesoBruto() ) );			// Peso bruto
	interfaz.establecerTextoEtiqueta( "HoraSalidaInterno", ticket -> obtenerHoraSalida() );						// Hora salida
	interfaz.establecerTextoEtiqueta( "PesoTaraInterno", to_string( ticket -> obtenerPesoTara() ) ) ;			// Peso tara
	interfaz.establecerTextoEntrada( "PermitirDescuentoInterno", ( ticket -> permitirDescuento() ? "Sí" : "No" ) );	// Descuento (permiso)
	interfaz.establecerTextoEntrada( "DescuentoInterno", ( ticket -> permitirDescuento() ? to_string( ticket -> obtenerDescuento() ) : "" ) );	// Descuento
	interfaz.establecerTextoEntrada( "TipoRegistroInterno", ( ticket -> obtenerTipoRegistro() == TIPO_REGISTRO_ENTRADA ? "Entrada" : "Salida" ) );	// Tipo de registro
	interfaz.establecerTextoEtiqueta( "PesoNetoInterno", to_string( ticket -> obtenerPesoNeto() ) );			// Peso neto
	interfaz.establecerTextoEntrada( "ObservacionesInterno", ticket -> obtenerObservaciones() );				// Observaciones

	// Señal de boton si
	interfaz.conectarSenal( botonSi, G_CALLBACK( internoEliminarSeleccionado ), nullptr );

	// Señal boton de eliminar registro interno seleccionado
	interfaz.conectarSenal( eliminarRegistroInterno, G_CALLBACK( internoAlertaEliminar ), nullptr );

	// Redirige a la vista
	irA( "PesajeInterno", false );
}

void vistaPublicoConsultarRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data )
{
	// Obtiene el ticket que se desea consultar
	registroPublico = buscarRegistroPublicoPorFolio( obtenerFolioSelector( row ), registrosPublicosConsultados );
	if( registroPublico == nullptr ){
		mostrarMensaje( "Registro no encontrado o no disponible." );
		return;
	}
	
	interfaz.establecerTextoEtiqueta( "FolioPublico", to_string( registroPublico -> obtenerFolio() ) );	// Folio
	interfaz.establecerTextoEtiqueta( "FechaPublico", registroPublico -> obtenerFecha() );				// Fecha
	interfaz.establecerTextoEntrada( "NombreProductoPublico", registroPublico -> obtenerProducto() -> obtenerNombre() ); // Producto
	interfaz.establecerTextoEntrada( "NombreConductorPublico", registroPublico -> obtenerNombreConductor() );// Nombre del conductor	
	interfaz.establecerTextoEntrada( "NumeroPlacasPublico", registroPublico -> obtenerNumeroPlacas() );// Número de placas
	interfaz.establecerTextoEtiqueta( "HoraEntradaPublico", registroPublico -> obtenerHoraEntrada() );// Hora entrada
	interfaz.establecerTextoEtiqueta( "PesoBrutoPublico", to_string( registroPublico -> obtenerPesoBruto() ) );// Peso bruto
	interfaz.establecerTextoEtiqueta( "HoraSalidaPublico", registroPublico -> obtenerHoraSalida() );// Hora salida
	interfaz.establecerTextoEtiqueta( "PesoTaraPublico", to_string( registroPublico -> obtenerPesoTara() ) ) ;// Peso tara
	interfaz.establecerTextoEntrada( "TipoViajePublico", ( registroPublico -> obtenerTipoViaje() == VIAJE_LOCAL ? "Local" : "Foráneo" ) );// Tipo de registro
	interfaz.establecerTextoEtiqueta( "PesoNetoPublico", to_string( registroPublico -> obtenerPesoNeto() ) );// Peso neto

	// Señal boton de eliminar registro interno seleccionado
	interfaz.conectarSenal( eliminarRegistroInterno, G_CALLBACK( internoAlertaEliminar ), nullptr );

	// Señal de boton si
	interfaz.conectarSenal( botonSi, G_CALLBACK( publicoEliminarSeleccionado ), nullptr );

	// Señal boton imprimir interno
	interfaz.conectarSenal( imprimirRegistroPublico, G_CALLBACK( publicoImprimirSeleccionado ), nullptr );

	// Redirige a la vista
	irA( "PesajePublico", false );
}

void vistaConsultarUsuarios()
{
	// Obtiene todos los usuarios registrados
	obtenerUsuariosRegistrados();

	// Establece el contenedor de usuarios registrados
	actualizarRegistrosUsuarios( registrosUsuarios, "ContenedorUsuarios" );

	irHacia( nullptr, (void *)"ConsultarUsuarios" );
}

void regresarInicio()
{
	// No se puede consultar a ningún usurio que hubiera sido consultado
	usuarioConsultado = nullptr;

	// No se puede acceder a la lista de usuarios registrados fuera de esa vista
	limpiarUsuariosRegistrados( registrosUsuarios );

	// Regresa a la vista de inicio
	irHacia( nullptr, (void *)"Inicio" );
}

void vistaConsultarUsuario()
{
	usuarioConsultado = buscarUsuarioPorNombreUsuario( interfaz.obtenerTextoEntrada( "EntradaConsultarUsuario" ), registrosUsuarios );
	if( usuarioConsultado == nullptr ){
		mostrarMensaje( "El usuario que se desea consultar no fue encontrado." );
		return;
	}

	// Nombre
	interfaz.establecerTextoEntrada( "NombreConsultado", usuarioConsultado -> obtenerNombre() );

	// Apellidos
	interfaz.establecerTextoEntrada( "ApellidosConsultado", usuarioConsultado -> obtenerApellidos() );

	// Nombre de usuario
	interfaz.establecerTextoEntrada( "NombreUsuarioConsultado", usuarioConsultado -> obtenerNombreUsuario() );

	// Administrador
	if( usuarioConsultado -> esAdministrador() ){
		interfaz.establecerActivoBotonToggle( "UsuarioAdministrador" );
	}

	//Señal Eliminar usuarios	
	interfaz.conectarSenal( botonEliminarUsuario, G_CALLBACK( alerta ), (void *)"¿Estás seguro que deseas eliminar este usuario?" );

	// Boton si si decide eliminar
	interfaz.conectarSenal( botonSi, G_CALLBACK( eliminarUsuario ), nullptr );

	irHacia( nullptr, (void *)"InformacionUsuario" );
}

void regresarUsuarios()
{
	// No se puede acceder a la información del usuario fuera de esa vista
	usuarioConsultado = nullptr;

	// Regresa a la vista de inicio
	irHacia( nullptr, (void *)"ConsultarUsuarios" );
}

void internoLimpiarFormulario()
{
	interfaz.establecerTextoEntrada( "EntradaNombreEmpresaInterno", "" );				// Empresa
	interfaz.establecerTextoEntrada( "EntradaNombreProductoInterno", "" );				// Producto
	interfaz.establecerTextoEntrada( "EntradaNombreConductorInterno", "" );				// Nombre del conductor
	interfaz.establecerTextoEntrada( "EntradaNumeroPlacasInterno", "" );				// Numero de placas
	interfaz.establecerTextoEtiqueta( "EntradaHoraEntradaInterno", "No establecida" );	// Hora entrada
	interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoInterno", "No establecido" );	// Peso bruto
	interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", "No establecida" );	// Hora salida
	interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", "No establecido" );		// Peso tara
	interfaz.establecerTextoEtiqueta( "EntradaPesoNetoInterno", "No establecido" );		// Peso neto
	interfaz.establecerTextoEntrada( "EntradaObservacionesInterno", "" );				// Observaciones
}

void vistaBascula( GtkListBox *box, GtkListBoxRow *row, gpointer data )
{
	// Obtiene la bascula seleccionada
	Bascula *bascula = basculaBuscarPorCodigo( obtenerFolioSelector( row ) );
	if( bascula == nullptr ){
		throw runtime_error( "Ocurrió un error al intentar recuperar la bascula seleccionada." );
	}

	// Establece las opciones que indica la bascula
	interfaz.establecerTextoEtiqueta( "OpcionesBasculaCodigo", to_string( obtenerFolioSelector( row ) ) );
	interfaz.establecerTextoEntrada( "OpcionesBasculaNombre", bascula -> obtenerNombre() );
	interfaz.establecerActivoComboBoxText( "OpcionesBasculaPuerto", bascula -> obtenerPuerto() );
	interfaz.establecerTextoEntrada( "OpcionesBasculaBitsDatos", to_string( bascula -> obtenerByteSize() ) );
	interfaz.establecerTextoEntrada( "OpcionesBasculaBitsStop", to_string( bascula -> obtenerStopBits() ) );
	interfaz.establecerTextoEntrada( "OpcionesBasculaBytesIgnorados", to_string( bascula -> obtenerBytesIgnorados() ) );

	// Redirige hacia la vista
	irA( "OpcionesBascula", false );
}
