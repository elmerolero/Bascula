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
#include "GestorRegistros.h"
#include "RegistrosInternos.h"
#include "RegistrosPublicos.h"
using namespace std;

Signal botonBasculaNuevo{ "BotonBasculaNuevo", "clicked", 0 };
Signal entradaSeguimiento{ "EntradaSeguimiento", "activate", 0 };
Signal botonSeguimiento{ "BotonSeguimiento", "clicked", 0 };

guint botonCancelarPublicoId = 0;

Signal botonNuevoRegistro{ "BotonNuevoRegistro", "clicked", 0 };
Signal botonNuevoRegistro2{ "BotonNuevoRegistro2", "clicked", 0 };
Signal entradaConsultarNombre{ "EntradaConsultarNombre", "activate", 0 };
Signal botonConsultarNombre{ "BotonConsultarNombre", "clicked", 0 };
Signal enlaceRegresarRegistros{ "EnlaceConsultarRegistroRegresar", "activate-link", 0 };
Signal enlaceRegresarRegistros2{ "EnlaceConsultarRegistroRegresar", "activate-link", 0 };

Signal botonGuardarEdicionRegistro{ "BotonGuardarEdicionRegistro", "clicked", 0 };
Signal botonGuardarNuevoRegistro{ "BotonGuardarNuevoRegistro", "clicked", 0 };
Signal botonCancelarNuevoRegistro{ "BotonCancelarNuevoRegistro", "clicked", 0 };
Signal botonSi{ "BotonSi", "clicked", 0 };

Signal enlaceRegistrosPesajeRegresar{ "EnlaceRegistrosPesajeRegresar", "activate-link", 0 };
Signal botonRegistrosPesajeSeleccionarDia{ "BotonRegistrosPesajeSeleccionarDia", "clicked", 0 };
Signal botonRegistrosPesajeObtenerInforme{ "BotonRegistrosPesajeObtenerInforme", "clicked", 0 };
Signal entradaRegistrosPesajeFolio{ "EntradaRegistrosPesajeFolio", "activate", 0 };
Signal botonRegistrosPesajeConsultarRegistro{ "BotonRegistrosPesajeConsultarRegistro", "clicked" };

Signal botonConsultarDia{ "BotonConsultarDia", "clicked", 0 };
Signal botonObtenerRegistrosRango{ "BotonObtenerRegistrosRango", "clicked", 0 };
Signal botonObtenerRegistros{ "BotonObtenerRegistros", "clicked", 0 };
Signal enlaceRegistrosPesajeRegresarConsulta{ "EnlaceRegistrosPesajeRegresarConsulta", "activate-link", 0 };
Signal enlaceRegistrosPesajeRegresarInforme{ "EnlaceRegistrosPesajeRegresarInforme", "activate-link", 0 };

Signal imprimirRegistroPublico{ "ImprimirRegistroPublico", "clicked", 0 }; // Apto para ser movido hacia Aplicacion.cpp
Signal eliminarRegistro{ "EliminarRegistroInterno", "clicked", 0 };

Signal botonEliminarUsuario{ "BotonEliminarUsuario", "clicked", 0 };
Signal botonCambiarContrasena{ "BotonCambiarContrasena", "clicked", 0 };
Signal entradaReemplazarConfirmacion{ "EntradaReemplazarConfirmacion", "activate", 0 };

void vistaCuenta( GtkWidget *widget, gpointer ptr )
{
	irHacia( nullptr, (void *)"Cuenta" );
	
	interfaz.establecerTextoEntrada( "EntradaCuentaNombre", usuario.obtenerNombre() );
	interfaz.establecerTextoEntrada( "EntradaCuentaApellidos", usuario.obtenerApellidos() );
	interfaz.establecerTextoEntrada( "EntradaCuentaNombreUsuario", usuario.obtenerNombreUsuario() );
	interfaz.establecerTextoEntrada( "EntradaCuentaContrasenaNueva", "" );
	interfaz.establecerTextoEntrada( "EntradaCuentaContrasenaConfirmacion", "" );
}

void vistaNuevoRegistro( GtkWidget *widget, gpointer ptr )
{
	// Va hacia la vista de consulta de registros
	irHacia( nullptr, (void *)"ConsultarRegistro" );
	
	// Oculta los datos que tendría un registro seleccionado y muestra el formulario de nuevo registro
	interfaz.ocultarElemento( "Registro" );
	interfaz.mostrarElemento( "NuevoRegistro" );
	
	// Establece el título de nuevo registro
	interfaz.establecerTextoEtiqueta( "TiuloRegistro1", "Nuevo Registro" );
	
	// Desconecta señales
	interfaz.ocultarElemento( "ControlesGestionRegistros" );
}

void vistaRegistrosEmpresas( GtkWidget *widget, gpointer ptr )
{
	// Establece la vista para consultar registros
	irHacia( nullptr, (void *)"ConsultarRegistros" );
	
	// Establece el nombre de los registros que se estan consultando
	interfaz.establecerTextoEtiqueta( "TituloRegistros", "Empresas" );
	
	// Establece el nombre de los registros que se estan consultando
	interfaz.establecerTextoEtiqueta( "TituloRegistro", "Empresa" );
	
	// Establece la imagen de registro
	interfaz.establecerImagen( "ImagenRegistro", "../resources/images/icons/Empresas.png" );
	
	// Actualiza la lista de registros de empresas
	empresas.actualizarListaRegistros();
	
	// Conecta las señales de la vista 
	interfaz.conectarSenal( botonNuevoRegistro, G_CALLBACK( vistaNuevoRegistro ), nullptr );
	interfaz.conectarSenal( entradaConsultarNombre, G_CALLBACK( vistaRegistroEmpresa ), nullptr );
	interfaz.conectarSenal( botonConsultarNombre, G_CALLBACK( vistaRegistroEmpresa ), nullptr );
	interfaz.conectarSenal( enlaceRegresarRegistros, G_CALLBACK( vistaRegistrosEmpresas ), nullptr );
	interfaz.conectarSenal( botonNuevoRegistro2, G_CALLBACK( vistaNuevoRegistro ), nullptr );
    interfaz.conectarSenal( botonGuardarEdicionRegistro, G_CALLBACK( actualizarEmpresa ), nullptr );
    interfaz.conectarSenal( botonGuardarNuevoRegistro, G_CALLBACK( nuevoEmpresa ), nullptr );
    interfaz.conectarSenal( botonCancelarNuevoRegistro, G_CALLBACK( vistaRegistrosEmpresas ), nullptr );
    interfaz.conectarSenal( botonSi, G_CALLBACK( eliminarEmpresa ), nullptr );
    
    // Establece el completador de empresas
    interfaz.establecerCompletadorEntrada( "EntradaNombreEmpresaInterno", NULL );
    interfaz.establecerCompletadorEntrada( "EntradaConsultarNombre", empresas.obtenerCompletador() );
}

void vistaRegistroEmpresa( GtkWidget *widget, gpointer ptr )
{
	// Obtiene el nombre del registro a consultar
	registro = empresas.buscarRegistroPorNombre( interfaz.obtenerTextoEntrada( "EntradaConsultarNombre" ) );
	if( registro == nullptr ){
		mostrarMensaje( "Registro no encontrado." );
		return;
	}
	
	// Llama a vista registro con el registro indicado
	vistaRegistro( registro );
}

void vistaRegistrosProductos( GtkWidget *widget, gpointer ptr )
{
	// Establece la vista de consultar registros
	irHacia( nullptr, (void *)"ConsultarRegistros" );
	
	// Establece el nombre de los registros que se estan consultando
	interfaz.establecerTextoEtiqueta( "TituloRegistros", "Productos" );

	// Establece el nombre de los registros que se estan consultando
	interfaz.establecerTextoEtiqueta( "TituloRegistro", "Producto" );
	
	// Establece la imagen de registro
	interfaz.establecerImagen( "ImagenRegistro", "../resources/images/icons/Producto.png" );
	
	// Actualiza la lista de registros de empresas
	productos.actualizarListaRegistros();
	
	// Conecta las señales de la vista
	interfaz.conectarSenal( botonNuevoRegistro, G_CALLBACK( vistaNuevoRegistro ), nullptr );
	interfaz.conectarSenal( entradaConsultarNombre, G_CALLBACK( vistaRegistroProducto ), nullptr );
	interfaz.conectarSenal( botonConsultarNombre, G_CALLBACK( vistaRegistroProducto ), nullptr );
    interfaz.conectarSenal( enlaceRegresarRegistros2, G_CALLBACK( vistaRegistrosProductos ), nullptr );
    interfaz.conectarSenal( botonNuevoRegistro2, G_CALLBACK( vistaNuevoRegistro ), nullptr );
    interfaz.conectarSenal( botonGuardarEdicionRegistro, G_CALLBACK( actualizarProducto ), nullptr );
    interfaz.conectarSenal( botonGuardarNuevoRegistro, G_CALLBACK( nuevoProducto ), nullptr );
    interfaz.conectarSenal( botonCancelarNuevoRegistro, G_CALLBACK( vistaRegistrosProductos ), nullptr );
    interfaz.conectarSenal( botonSi, G_CALLBACK( eliminarProducto ), nullptr );

	// Establece el completador de empresas
    interfaz.establecerCompletadorEntrada( "EntradaNombreProductoInterno", NULL );
    interfaz.establecerCompletadorEntrada( "EntradaNombreProductoPublico", NULL );
    interfaz.establecerCompletadorEntrada( "EntradaConsultarNombre", productos.obtenerCompletador() );
}

void vistaRegistroProducto( GtkWidget *widget, gpointer ptr )
{
	// Obtiene el nombre del registro a consultar
	registro = productos.buscarRegistroPorNombre( interfaz.obtenerTextoEntrada( "EntradaConsultarNombre" ) );
	if( registro == nullptr ){
		mostrarMensaje( "Registro no encontrado." );
		return;
	}
	
	// Llama a vista registro
	vistaRegistro( registro );
}

void vistaRegistro( Registro *registro )
{
	// Establece la vista de registro
	irHacia( nullptr, (void *)"ConsultarRegistro" );
	
	// Muestra el control de registros
	interfaz.mostrarElemento( "ControlesGestionRegistros" );
	
	// Oculta los datos que tendría un registro seleccionado y muestra el formulario de nuevo registro
	interfaz.ocultarElemento( "NuevoRegistro" );
	interfaz.mostrarElemento( "Registro" );
	
	// Oculta el campo de entrada del nombre y los controles de edición
	interfaz.ocultarElemento( "EntradaNombreRegistro" );
	interfaz.ocultarElemento( "ControlesEdicionRegistro" );
	
	// Muestra la etiqueta con el nombre del registro
	interfaz.mostrarElemento( "EtiquetaNombreRegistro" );
	
	// Establece la clave del registro
	stringstream clave;
	clave << setfill( '0' ) << setw( 7 ) << ( registro -> obtenerClave() );
	interfaz.establecerTextoEtiqueta( "EtiquetaClaveRegistro", clave.str() );
	
	// Establece el nombre del registro
	interfaz.establecerTextoEtiqueta( "EtiquetaNombreRegistro", registro -> obtenerNombre() );
	interfaz.establecerTextoEntrada( "EntradaNombreRegistro", registro -> obtenerNombre() );
}

void vistaRegistroEditar( GtkWidget *widget, gpointer ptr )
{	
	// Oculta la etiqueta con el nombre del registro
	interfaz.ocultarElemento( "EtiquetaNombreRegistro" );
	
	// Muestra la entrada de edición con el nombre del registro
	interfaz.mostrarElemento( "EntradaNombreRegistro" );
	
	// Muestra los botones de edicion de la interfaz
	interfaz.mostrarElemento( "ControlesEdicionRegistro" );
}

void vistaConfiguracion( GtkWidget *widget, gpointer ptr )
{
	// Oculta el mensaje de error
	interfaz.ocultarElemento( "MensajeErrorConfiguracion" );

	// Establece el baudrate establecido
	interfaz.establecerActivoComboBoxText( "OpcionesBaudrate", "CBR_" + to_string( lectorBascula.obtenerBaudRate() ) );

	// Establece los bits de datos
	interfaz.establecerTextoEntrada( "OpcionesBitsDatos", to_string( lectorBascula.obtenerByteSize() ) );

	// Establece los bits de stop
	interfaz.establecerTextoEntrada( "OpcionesBitsStop", to_string( lectorBascula.obtenerStopBits() ) );

	// Establecer parity
	interfaz.establecerActivoComboBoxText( "OpcionesParidad", parityOptions[ lectorBascula.obtenerParity() ] );

	// Bits a ignorar
	interfaz.establecerTextoEntrada( "OpcionesBytesIgnorados", to_string( lectorBascula.obtenerBytesIgnorados() ) );

	// Numero copias
	interfaz.establecerTextoEntrada( "OpcionesImpresionFormatos", to_string( numeroFormatos ) );
	interfaz.establecerTextoEntrada( "OpcionesImpresionCopias", to_string( numeroCopias ) );

	// Se dirige a la vista
	irHacia( nullptr, (void *)"Configuracion" );
}

void vistaBasculaPublica( GtkWidget *widget, gpointer ptr )
{
	// Establece las vistas
	irHacia( nullptr, (void *)"Tickets" );
	
	// Obtiene los tickets pendiente
	publicoActualizarRegistros( registrosPublicosPendientes, "ContenedorTickets" );
	
	// Conecta las señales correspondientes
    interfaz.conectarSenal( botonBasculaNuevo, G_CALLBACK( vistaCrearRegistroPublico ), nullptr );
    interfaz.conectarSenal( entradaSeguimiento, G_CALLBACK( vistaFinalizarRegistroPublico ), nullptr );
    interfaz.conectarSenal( botonSeguimiento, G_CALLBACK( vistaFinalizarRegistroPublico ), nullptr );
}

void vistaBasculaInterna( GtkWidget *widget, gpointer ptr )
{	
	// Obtiene los tickets pendiente
	internoActualizarRegistros( registrosInternosPendientes, "ContenedorTickets" );
	
	// Oculta la barra de mensajes
	interfaz.ocultarElemento( "MensajeTicketsPendientes" );
	
	// Conecta las señales correspondientes
    interfaz.conectarSenal( botonBasculaNuevo, G_CALLBACK( vistaCrearRegistro ), nullptr );
    interfaz.conectarSenal( entradaSeguimiento, G_CALLBACK( vistaInternoEditarRegistro ), nullptr );
    interfaz.conectarSenal( botonSeguimiento, G_CALLBACK( vistaInternoEditarRegistro ), nullptr );

    // Establece las vistas
	irHacia( nullptr, (void *)"Tickets" );
}

void vistaCrearRegistroPublico( GtkWidget *widget, gpointer ptr )
{	
	// Oculta los errores que pudieron haber ocurrido en registros anteriores
	interfaz.ocultarElemento( "MensajeErrorCampoPublico" );
	
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
	interfaz.establecerCompletadorEntrada( "EntradaConsultarNombre", NULL );
	interfaz.establecerCompletadorEntrada( "EntradaNombreProductoInterno", NULL );
	interfaz.establecerCompletadorEntrada( "EntradaNombreProductoPublico", productos.obtenerCompletador() ); 
	
	// Establece la señal para cancelar el registro
	interfaz.desconectarSenal( "EnlaceRegresarPublico", botonCancelarPublicoId );
	botonCancelarPublicoId = interfaz.conectarSenal( "EnlaceRegresarPublico", "activate-link", G_CALLBACK( publicoCancelarRegistro ), nullptr );
	
	// Establece la vista de nuevo ticket
	irHacia( nullptr, (void *)"NuevoTicketPublico" );
}

void vistaFinalizarRegistroPublico()
{
	// Busca el ticket
	try{
		registroPublico = buscarRegistroPublicoPorFolio( stoi( interfaz.obtenerTextoEntrada( "EntradaSeguimiento" ) ), registrosPublicosPendientes );
	}
	catch( invalid_argument &ia ){
		mostrarMensaje( "Introduzca un folio válido." );
		return;
	} 
	
	// Si no encuentra el ticket
	if( registroPublico == nullptr ){
		interfaz.establecerTextoEtiqueta( "DialogoMensaje", "Registro no encontrado." );
		interfaz.mostrarElemento( "VentanaMensaje" );
		return;
	}
	
	// Oculta los errores que pudieron haber ocurrido en registros anteriores
	interfaz.ocultarElemento( "MensajeErrorCampo" );
	
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
	
	// Establece la señal para cancelar el registro
	interfaz.desconectarSenal( "EnlaceRegresarPublico", botonCancelarPublicoId );
	botonCancelarPublicoId = interfaz.conectarSenal( "EnlaceRegresarPublico", "activate-link", G_CALLBACK( publicoCancelarFinalizacion ), nullptr );
	
	// Establece el texto de finalizar
	interfaz.establecerBotonEtiqueta( "BotonRegistrarPublico", "Finalizar" );

	// Va hacia la vista
	irHacia( nullptr, (void *)"NuevoTicketPublico" );
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
	
	// Limpia el formulario
	internoLimpiarFormulario();

	// Descuento
	interfaz.establecerActivoBotonToggle( "NoDescuentoInterno" );
	interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", "" );
	interfaz.deshabilitarEdicionEntrada( "EntradaDescuentoInterno" );

	// Tipo de registro (registra entrada por defecto)
	interfaz.establecerActivoBotonToggle( "RegistraEntrada" );
	
	// Establece el completador de empresa
	interfaz.establecerCompletadorEntrada( "EntradaConsultarNombre", NULL );
	interfaz.establecerCompletadorEntrada( "EntradaNombreEmpresaInterno", empresas.obtenerCompletador() );
	
	// Establece el completador de producto
	interfaz.establecerCompletadorEntrada( "EntradaConsultarNombre", NULL );
	interfaz.establecerCompletadorEntrada( "EntradaNombreProductoPublico", NULL );
	interfaz.establecerCompletadorEntrada( "EntradaNombreProductoInterno", productos.obtenerCompletador() );
	
	// Es un ticket pendiente hasta que no se demuestre lo contrario
	// internoPendiente = true;

	// Establece la vista de nuevo ticket 
	irHacia( nullptr, (void *)"NuevoTicketInterno" );
}

void vistaInternoEditarRegistro()
{
	Ticket *ticket;
	try{
		ticket = buscarRegistroInternoPorFolio( stoi( interfaz.obtenerTextoEntrada( "EntradaSeguimiento" ) ), registrosInternosPendientes );
	}
	catch( invalid_argument &ia ){
		mostrarMensaje( "Introduzca un folio válido." );
		return;
	} 
	
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
	interfaz.ocultarElemento( "MensajeErrorCampo" );
	lectorBascula.abrir( interfaz );
	lectorBascula.establecerIdSenal( interfaz.conectarSenal( "BotonRegistrarPeso", "clicked", G_CALLBACK( internoRegistrarPesoBruto ), nullptr ) );
}

void vistaLeerPesoTara()
{
	interfaz.ocultarElemento( "MensajeErrorCampo" );
	lectorBascula.abrir( interfaz );
	lectorBascula.establecerIdSenal( interfaz.conectarSenal( "BotonRegistrarPeso", "clicked", G_CALLBACK( internoRegistrarPesoTara ), nullptr ) );
}

void vistaLeerPesoBrutoPublico()
{
	interfaz.ocultarElemento( "MensajeErrorCampoPublico" );
	lectorBascula.abrir( interfaz );
	lectorBascula.establecerIdSenal( interfaz.conectarSenal( "BotonRegistrarPeso", "clicked", G_CALLBACK( publicoRegistrarPesoBruto ), nullptr ) );
}

void vistaLeerPesoTaraPublico()
{
	interfaz.ocultarElemento( "MensajeErrorCampoPublico" );
	string pesoBruto = interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoPublico" );
	lectorBascula.abrir( interfaz );
	lectorBascula.establecerIdSenal( interfaz.conectarSenal( "BotonRegistrarPeso", "clicked", G_CALLBACK( publicoRegistrarPesoTara ), nullptr ) );
}

void vistaConsultarPesajesInternos()
{
	// Muestra los registros del día
	internoObtenerPorFecha( registrosInternosConsultados, obtenerFecha() );
	internoActualizarRegistros( registrosInternosConsultados, "ContenedorRegistrosPesaje" );

	// Oculta la barra de mensajes
	interfaz.ocultarElemento( "MensajeTicketsPendientes" );

	// Establece la fecha del ticket que se está consultando
	interfaz.establecerTextoEtiqueta( "TicketsRegistrados", "Registros del día " + obtenerFecha() + ":"  );

	// Establece el número de tickets
	interfaz.establecerTextoEtiqueta( "TicketsContados", to_string( registrosInternosConsultados.size() ) + " registros"  );

	// Señal de retorno consultar pesajes internos
	interfaz.conectarSenal( enlaceRegistrosPesajeRegresar, G_CALLBACK( irHacia ), (void *)"Registros" );

	// Señal de retorno consultar dia
	interfaz.conectarSenal( enlaceRegistrosPesajeRegresarConsulta, G_CALLBACK( irHacia ), (void *)"Pesajes" );

	// Señal del boton seleccionar día
	interfaz.conectarSenal( botonRegistrosPesajeSeleccionarDia, G_CALLBACK( irHacia ), ( void *)"ConsultarDia" );

	// Señal del boton informe
	interfaz.conectarSenal( botonRegistrosPesajeObtenerInforme, G_CALLBACK( internoGenerarInforme ), nullptr );

	// Señal de obtener registros
	interfaz.conectarSenal( botonObtenerRegistros, G_CALLBACK( irHacia ), (void *)"ObtenerRegistros" );

	// Señal de selección de ticket (entrada)
	interfaz.conectarSenal( entradaRegistrosPesajeFolio, G_CALLBACK( vistaConsultarRegistroInterno ), nullptr );

	// Señal de selección de ticket (boton)
	interfaz.conectarSenal( botonRegistrosPesajeConsultarRegistro, G_CALLBACK( vistaConsultarRegistroInterno ), nullptr );

	// Señal de retorno vista generar informe
	interfaz.conectarSenal( enlaceRegistrosPesajeRegresarInforme, G_CALLBACK( irHacia ), (void *)"Pesajes" );

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

	// Señal de retorno consultar pesajes internos
	interfaz.conectarSenal( enlaceRegistrosPesajeRegresar, G_CALLBACK( irHacia ), (void *)"Registros" );

	// Señal de retorno consultar dia
	interfaz.conectarSenal( enlaceRegistrosPesajeRegresarConsulta, G_CALLBACK( irHacia ), (void *)"Pesajes" );

	// Señal del boton seleccionar día
	interfaz.conectarSenal( botonRegistrosPesajeSeleccionarDia, G_CALLBACK( irHacia ), ( void *)"ConsultarDia" );

	// Señal del boton informe
	interfaz.conectarSenal( botonRegistrosPesajeObtenerInforme, G_CALLBACK( publicoGenerarInforme ), nullptr );

	// Señal de obtener registros
	interfaz.conectarSenal( botonObtenerRegistros, G_CALLBACK( irHacia ), (void *)"ObtenerRegistros" );	

	// Señal de selección de ticket (entrada)
	interfaz.conectarSenal( entradaRegistrosPesajeFolio, G_CALLBACK( vistaConsultarRegistroPublico ), nullptr );

	// Señal de selección de ticket (boton)
	interfaz.conectarSenal( botonRegistrosPesajeConsultarRegistro, G_CALLBACK( vistaConsultarRegistroPublico ), nullptr );

	// Señal de retorno vista generar informe
	interfaz.conectarSenal( enlaceRegistrosPesajeRegresarInforme, G_CALLBACK( irHacia ), (void *)"Pesajes" );

	// Boton de seleccion dia
	interfaz.conectarSenal( botonConsultarDia, G_CALLBACK( publicoSeleccionarDia ), nullptr );

	// Boton de generación de informe
	interfaz.conectarSenal( botonObtenerRegistrosRango, G_CALLBACK( publicoObtenerRegistrosRango ), nullptr );

	// Redirige hacia la vista
	irHacia( nullptr, (void *)"Pesajes" );
}

void vistaConsultarRegistroInterno()
{
	// Ticket para obtener la información
	Ticket *ticket;

	// Obtiene el ticket que se desea consultar
	try{
		ticket = buscarRegistroInternoPorFolio( stoi( interfaz.obtenerTextoEntrada( "EntradaRegistrosPesajeFolio" ) ), registrosInternosConsultados );
	}
	catch( invalid_argument &ia ){
		mostrarMensaje( "Introduzca un folio válido." );
		return;
	}

	// Si el ticket no es encontrado
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
	interfaz.conectarSenal( eliminarRegistro, G_CALLBACK( internoAlertaEliminar ), nullptr );

	// Redirige a la vista
	irHacia( nullptr, (void *)"RegistroInterno" );
}

void vistaConsultarRegistroPublico()
{
	// Obtiene el ticket que se desea consultar
	try{
		registroPublico = buscarRegistroPublicoPorFolio( stoi( interfaz.obtenerTextoEntrada( "EntradaRegistrosPesajeFolio" ) ), registrosPublicosConsultados );
	}
	catch( invalid_argument &ia ){
		mostrarMensaje( "Introduzca un folio válido." );
		return;
	}

	// Si el ticket no es encontrado
	if( registroPublico == nullptr ){
		mostrarMensaje( "Registro no encontrado." );
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
	interfaz.conectarSenal( eliminarRegistro, G_CALLBACK( internoAlertaEliminar ), nullptr );

	// Señal de boton si
	interfaz.conectarSenal( botonSi, G_CALLBACK( publicoEliminarSeleccionado ), nullptr );

	// Señal boton imprimir interno
	interfaz.conectarSenal( imprimirRegistroPublico, G_CALLBACK( publicoImprimirSeleccionado ), nullptr );

	// Redirige a la vista
	irHacia( nullptr, (void *)"RegistroPublico" );
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

void vistaReemplazarContrasena()
{
	// Boton de registrar
	interfaz.conectarSenal( botonCambiarContrasena, G_CALLBACK( cambiarContrasenaUsuario ), nullptr );

	// Boton de confirmacion
	interfaz.conectarSenal( entradaReemplazarConfirmacion, G_CALLBACK( cambiarContrasenaUsuario ), nullptr );

	// Redirige hacia la vista
	irHacia( nullptr, (void *)"ReemplazarContrasena" );
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
