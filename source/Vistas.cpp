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

guint botonBasculaNuevoId = 0;
guint botonSeguimientoId = 0;
guint entradaSeguimientoId = 0;

guint botonLeerPesoBrutoInternoId = 0;
guint botonLeerPesoTaraInternoId = 0;
guint botonLeerPesoBrutoPublicoId = 0;
guint botonLeerPesoTaraPublicoId = 0;

guint botonRegistrarTicketId = 0;
guint botonCancelarInternoId = 0;

guint botonRegistrarPublicoId = 0;
guint botonCancelarPublicoId = 0;

guint botonNuevoRegistroId = 0;
guint botonNuevoRegistroId2 = 0;
guint entradaConsultarNombreId = 0;
guint botonConsultarNombreId = 0;
guint botonRegresarRegistrosId = 0;
guint botonGuardarEdicionRegistroId = 0;
guint botonEliminarRegistroId = 0;
guint botonConsultarNuevoRegistroId = 0;
guint botonGuardarNuevoRegistroId = 0;
guint botonCancelarNuevoRegistroId = 0;
guint botonSiId = 0;

guint enlaceRegistrosPesajeRegresarId = 0;
guint botonRegistrosPesajeSeleccionarDiaId = 0;
guint botonRegistrosPesajeObtenerInformeId = 0;
guint entradaRegistrosPesajeFolioId = 0;
guint botonRegistrosPesajeConsultarRegistroId = 0;

guint botonConsultarDiaId = 0;
guint botonObtenerRegistrosRangoId = 0;
guint botonObtenerRegistrosId = 0;
guint enlaceRegistrosPesajeRegresarConsultaId = 0;
guint enlaceRegistrosPesajeRegresarInformeId = 0;

guint imprimirRegistroInternoId = 0;
guint eliminarRegistroInternoId = 0;

guint entradaConsultarUsuarioId = 0;
guint botonConsultarUsuarioId = 0;
guint enlaceConsultarUsuariosRegresarId = 0;
guint enlaceConsultarUsuarioRegresarId = 0;
guint botonObtenerCodigoRecuperacionId = 0;
guint botonEliminarUsuarioId = 0;
guint usuarioAdministradorId = 0;

guint botonCambiarContrasenaId = 0;
guint entradaReemplazarConfirmacionId = 0;

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
	// Establece la vista de consultar registros
	irHacia( nullptr, (void *)"ConsultarRegistros" );
	
	// Establece el nombre de los registros que se estan consultando
	interfaz.establecerTextoEtiqueta( "TituloRegistros", "Empresas" );
	
	// Establece el nombre de los registros que se estan consultando
	interfaz.establecerTextoEtiqueta( "TituloRegistro", "Empresa" );
	
	// Establece la imagen de registro
	interfaz.establecerImagen( "ImagenRegistro", "../resources/images/icons/Empresas.png" );
	
	// Actualiza la lista de registros de empresas
	empresas.actualizarListaRegistros();
	
	// Desconecta las señales a las que podría haberse conectado antes
	interfaz.desconectarSenal( "BotonNuevoRegistro", botonNuevoRegistroId );
	interfaz.desconectarSenal( "EntradaConsultarNombre", entradaConsultarNombreId );
	interfaz.desconectarSenal( "BotonConsultarNombre", botonConsultarNombreId );
	
	interfaz.desconectarSenal( "EnlaceConsultarRegistroRegresar", botonRegresarRegistrosId );
	interfaz.desconectarSenal( "BotonNuevoRegistro2", botonNuevoRegistroId2 );
	interfaz.desconectarSenal( "BotonGuardarEdicionRegistro", botonGuardarEdicionRegistroId );
	interfaz.desconectarSenal( "BotonGuardarNuevoRegistro", botonGuardarNuevoRegistroId );
	interfaz.desconectarSenal( "BotonCancelarNuevoRegistro", botonCancelarNuevoRegistroId );
	interfaz.desconectarSenal( "BotonSi", botonSiId );
	
	// Conecta las señales de la vista 
	botonNuevoRegistroId = interfaz.conectarSenal( "BotonNuevoRegistro", "clicked", G_CALLBACK( vistaNuevoRegistro ), nullptr );
	entradaConsultarNombreId = interfaz.conectarSenal( "EntradaConsultarNombre", "activate", G_CALLBACK( vistaRegistroEmpresa ), nullptr );
	botonConsultarNombreId = interfaz.conectarSenal( "BotonConsultarNombre", "clicked", G_CALLBACK( vistaRegistroEmpresa ), nullptr );
	botonRegresarRegistrosId = interfaz.conectarSenal( "EnlaceConsultarRegistroRegresar", "activate-link", G_CALLBACK( vistaRegistrosEmpresas ), nullptr );
	botonNuevoRegistroId2 = interfaz.conectarSenal( "BotonNuevoRegistro2", "clicked", G_CALLBACK( vistaNuevoRegistro ), nullptr );
    botonGuardarEdicionRegistroId = interfaz.conectarSenal( "BotonGuardarEdicionRegistro", "clicked", G_CALLBACK( actualizarEmpresa ), nullptr );
    botonGuardarNuevoRegistroId = interfaz.conectarSenal( "BotonGuardarNuevoRegistro", "clicked", G_CALLBACK( nuevoEmpresa ), nullptr );
    botonCancelarNuevoRegistroId = interfaz.conectarSenal( "BotonCancelarNuevoRegistro", "clicked", G_CALLBACK( vistaRegistrosEmpresas ), nullptr );
    botonSiId = interfaz.conectarSenal( "BotonSi", "clicked", G_CALLBACK( eliminarEmpresa ), nullptr );
    
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
	
	// Desconecta las señales a las que podría haberse conectado antes
	interfaz.desconectarSenal( "BotonNuevoRegistro", botonNuevoRegistroId );
	interfaz.desconectarSenal( "EntradaConsultarNombre", entradaConsultarNombreId );
	interfaz.desconectarSenal( "BotonConsultarNombre", botonConsultarNombreId );
	
	interfaz.desconectarSenal( "EnlaceConsultarRegistroRegresar", botonRegresarRegistrosId );
	interfaz.desconectarSenal( "BotonNuevoRegistro2", botonNuevoRegistroId2 );
	interfaz.desconectarSenal( "BotonGuardarEdicionRegistro", botonGuardarEdicionRegistroId );
	interfaz.desconectarSenal( "BotonGuardarNuevoRegistro", botonGuardarNuevoRegistroId );
	interfaz.desconectarSenal( "BotonCancelarNuevoRegistro", botonCancelarNuevoRegistroId );
	interfaz.desconectarSenal( "BotonSi", botonSiId );
	
	// Conecta las señales de la vista
	botonNuevoRegistroId = interfaz.conectarSenal( "BotonNuevoRegistro", "clicked", G_CALLBACK( vistaNuevoRegistro ), nullptr );
	entradaConsultarNombreId = interfaz.conectarSenal( "EntradaConsultarNombre", "activate", G_CALLBACK( vistaRegistroProducto ), nullptr );
	botonConsultarNombreId = interfaz.conectarSenal( "BotonConsultarNombre", "clicked", G_CALLBACK( vistaRegistroProducto ), nullptr );
    botonRegresarRegistrosId = interfaz.conectarSenal( "EnlaceConsultarRegistroRegresar", "activate-link", G_CALLBACK( vistaRegistrosProductos ), nullptr );
    botonNuevoRegistroId2 = interfaz.conectarSenal( "BotonNuevoRegistro2", "clicked", G_CALLBACK( vistaNuevoRegistro ), nullptr );
    botonGuardarEdicionRegistroId = interfaz.conectarSenal( "BotonGuardarEdicionRegistro", "clicked", G_CALLBACK( actualizarProducto ), nullptr );
    botonGuardarNuevoRegistroId = interfaz.conectarSenal( "BotonGuardarNuevoRegistro", "clicked", G_CALLBACK( nuevoProducto ), nullptr );
    botonCancelarNuevoRegistroId = interfaz.conectarSenal( "BotonCancelarNuevoRegistro", "clicked", G_CALLBACK( vistaRegistrosProductos ), nullptr );
    botonSiId = interfaz.conectarSenal( "BotonSi", "clicked", G_CALLBACK( eliminarProducto ), nullptr );

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
	
	// Desconecta las señales correspondientes
	interfaz.desconectarSenal( "BotonBasculaNuevo", botonBasculaNuevoId );
	interfaz.desconectarSenal( "EntradaSeguimiento", entradaSeguimientoId );
	interfaz.desconectarSenal( "BotonSeguimiento", botonSeguimientoId );
	
	// Conecta las señales correspondientes
    botonBasculaNuevoId = interfaz.conectarSenal( "BotonBasculaNuevo", "clicked", G_CALLBACK( vistaCrearRegistroPublico ), nullptr );
    entradaSeguimientoId = interfaz.conectarSenal( "EntradaSeguimiento", "activate", G_CALLBACK( vistaFinalizarRegistroPublico ), nullptr );
    botonSeguimientoId = interfaz.conectarSenal( "BotonSeguimiento", "clicked", G_CALLBACK( vistaFinalizarRegistroPublico ), nullptr );
}

void vistaBasculaInterna( GtkWidget *widget, gpointer ptr )
{	
	// Obtiene los tickets pendiente
	internoActualizarRegistros( registrosInternosPendientes, "ContenedorTickets" );
	
	// Desconecta las señales correspondientes
	interfaz.desconectarSenal( "BotonBasculaNuevo", botonBasculaNuevoId );
	interfaz.desconectarSenal( "EntradaSeguimiento", entradaSeguimientoId );
	interfaz.desconectarSenal( "BotonSeguimiento", botonSeguimientoId );
	
	// Conecta las señales correspondientes
    botonBasculaNuevoId = interfaz.conectarSenal( "BotonBasculaNuevo", "clicked", G_CALLBACK( vistaCrearRegistro ), nullptr );
    entradaSeguimientoId = interfaz.conectarSenal( "EntradaSeguimiento", "activate", G_CALLBACK( vistaFinalizarRegistro ), nullptr );
    botonSeguimientoId = interfaz.conectarSenal( "BotonSeguimiento", "clicked", G_CALLBACK( vistaFinalizarRegistro ), nullptr );

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
	
	// Establece la señal para leer peso bruto
	interfaz.desconectarSenal( "BotonLeerPesoBrutoPublico", botonLeerPesoBrutoPublicoId );
	botonLeerPesoBrutoPublicoId = interfaz.conectarSenal( "BotonLeerPesoBrutoPublico", "clicked", G_CALLBACK( vistaLeerPesoBrutoPublico ), nullptr );
	
	// Establece la señar para leer el peso tara
	interfaz.desconectarSenal( "BotonLeerPesoTaraPublico", botonLeerPesoTaraPublicoId );
	botonLeerPesoTaraPublicoId = interfaz.conectarSenal( "BotonLeerPesoTaraPublico", "clicked", G_CALLBACK( vistaLeerPesoTaraPublico ), nullptr );
	
	// Establece la señal para registrar el ticket
	interfaz.desconectarSenal( "BotonRegistrarPublico", botonRegistrarPublicoId );
	botonRegistrarPublicoId = interfaz.conectarSenal( "BotonRegistrarPublico", "clicked", G_CALLBACK( publicoRegistrarPendiente ), nullptr );
	
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
	if( registroPublico -> estaPesoBrutoEstablecido() ){
		// Deshabilita la señal para leer el peso bruto
		interfaz.desconectarSenal( "BotonLeerPesoBrutoPublico", botonLeerPesoBrutoPublicoId );
	}
	
	// Hora salida y peso tara
	interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaPublico", ( registroPublico -> estaPesoTaraEstablecido() ? registroPublico -> obtenerHoraSalida() : "No establecida" ) );
	interfaz.establecerTextoEtiqueta( "EntradaPesoTaraPublico", ( registroPublico -> estaPesoTaraEstablecido() ? pesoString( registroPublico -> obtenerPesoTara(), 2 ) : "No establecido" ) );
	
	// Peso neto
	interfaz.establecerTextoEtiqueta( "EntradaPesoNetoPublico", ( registroPublico -> estaPesoNetoEstablecido() ? pesoString( registroPublico -> obtenerPesoNeto(), 2 ) : "No establecido" ) );
	
	// Establece la señal para cancelar el registro
	interfaz.desconectarSenal( "EnlaceRegresarPublico", botonCancelarPublicoId );
	botonCancelarPublicoId = interfaz.conectarSenal( "EnlaceRegresarPublico", "activate-link", G_CALLBACK( publicoCancelarFinalizacion ), nullptr );

	// Señal leer el peso tara
	interfaz.desconectarSenal( "BotonLeerPesoTaraPublico", botonLeerPesoTaraPublicoId );
	botonLeerPesoTaraPublicoId = interfaz.conectarSenal( "BotonLeerPesoTaraPublico", "clicked", G_CALLBACK( vistaLeerPesoTaraPublico ), nullptr );
	
	// Establece la señal para registrar el ticket
	interfaz.desconectarSenal( "BotonRegistrarPublico", botonRegistrarPublicoId );
	botonRegistrarPublicoId = interfaz.conectarSenal( "BotonRegistrarPublico", "clicked", G_CALLBACK( publicoFinalizarPendiente ), nullptr );
	
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
	
	// Establece el completador de empresa
	interfaz.establecerCompletadorEntrada( "EntradaConsultarNombre", NULL );
	interfaz.establecerCompletadorEntrada( "EntradaNombreEmpresaInterno", empresas.obtenerCompletador() );
	
	// Establece el completador de producto
	interfaz.establecerCompletadorEntrada( "EntradaConsultarNombre", NULL );
	interfaz.establecerCompletadorEntrada( "EntradaNombreProductoPublico", NULL );
	interfaz.establecerCompletadorEntrada( "EntradaNombreProductoInterno", productos.obtenerCompletador() );
	
	// Señal peso bruto
	interfaz.desconectarSenal( "BotonLeerPesoBrutoInterno", botonLeerPesoBrutoInternoId );
	botonLeerPesoBrutoInternoId = interfaz.conectarSenal( "BotonLeerPesoBrutoInterno", "clicked", G_CALLBACK( vistaLeerPesoBruto ), nullptr );
	
	// Señal peso tara
	interfaz.desconectarSenal( "BotonLeerPesoTaraInterno", botonLeerPesoTaraInternoId );
	botonLeerPesoTaraInternoId = interfaz.conectarSenal( "BotonLeerPesoTaraInterno", "clicked", G_CALLBACK( vistaLeerPesoTara ), nullptr);

	// Establece la señal para registrar el ticket
	interfaz.desconectarSenal( "BotonRegistrarInterno", botonRegistrarTicketId );
	botonRegistrarTicketId = interfaz.conectarSenal( "BotonRegistrarInterno", "clicked", G_CALLBACK( internoRegistrarPendiente ), nullptr );
	
	// Establece la señal para cancelar el registro
	interfaz.desconectarSenal( "EnlaceRegresarInterno", botonCancelarInternoId );
	botonCancelarInternoId = interfaz.conectarSenal( "EnlaceRegresarInterno", "activate-link", G_CALLBACK( internoCancelarRegistro ), nullptr );
	
	// Es un ticket pendiente hasta que no se demuestre lo contrario
	internoPendiente = true;

	// Establece la vista de nuevo ticket
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

void vistaFinalizarRegistro()
{
	// Busca el ticket
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
	interfaz.establecerTextoEtiqueta( "EntradaPesoBrutoInterno", ( ticket -> estaPesoBrutoEstablecido() ? pesoString( ticket -> obtenerPesoBruto(), 2 ) : "No establecido" ) );
	if( ticket -> estaPesoBrutoEstablecido() ){
		// Deshabilita la señal para leer el peso bruto
		interfaz.desconectarSenal( "BotonLeerPesoBrutoInterno", botonLeerPesoBrutoInternoId );
	}
	
	// Hora salida y peso tara
	interfaz.establecerTextoEtiqueta( "EntradaHoraSalidaInterno", ( ticket -> estaPesoTaraEstablecido() ? ticket -> obtenerHoraSalida() : "No establecida" ) );
	interfaz.establecerTextoEtiqueta( "EntradaPesoTaraInterno", ( ticket -> estaPesoTaraEstablecido() ? pesoString( ticket -> obtenerPesoTara(), 2 ) : "No establecido" ) );
	
	// Descuento
	if( ticket -> permitirDescuento() ){
		interfaz.establecerActivoBotonToggle( "SiDescuentoInterno" );
		interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", to_string( ticket -> obtenerDescuento() ) );
	}
	else{
		interfaz.establecerActivoBotonToggle( "NoDescuentoInterno" );
		interfaz.establecerTextoEntrada( "EntradaDescuentoInterno", "" );
	}
		
	// Motivos y observaciones
	interfaz.establecerTextoEntrada( "EntradaObservacionesInterno", ticket -> obtenerObservaciones() );

	// Señal leer peso tara
	interfaz.desconectarSenal( "BotonLeerPesoTaraInterno", botonLeerPesoTaraInternoId );
	botonLeerPesoTaraInternoId = interfaz.conectarSenal( "BotonLeerPesoTaraInterno", "clicked", G_CALLBACK( vistaLeerPesoTara ), nullptr);
	
	// Boton cancelar
	interfaz.desconectarSenal( "EnlaceRegresarInterno", botonCancelarInternoId );
	botonCancelarInternoId = interfaz.conectarSenal( "EnlaceRegresarInterno", "activate-link", G_CALLBACK( internoCancelarFinalizacion ), nullptr );
	
	// Boton registrar
	interfaz.desconectarSenal( "BotonRegistrarInterno", botonRegistrarTicketId );
	botonRegistrarTicketId = interfaz.conectarSenal( "BotonRegistrarInterno", "clicked", G_CALLBACK( internoFinalizarPendiente ), nullptr );
	
	// 
	interfaz.establecerBotonEtiqueta( "BotonRegistrarInterno", "Finalizar" );

	// Va hacia la vista
	irHacia( nullptr, (void *)"NuevoTicketInterno" );
}

void vistaLeerPesoBrutoPublico()
{
	interfaz.ocultarElemento( "MensajeErrorCampoPublico" );
	lectorBascula.abrir( interfaz );
	lectorBascula.establecerIdSenal( interfaz.conectarSenal( "BotonRegistrarPeso", "clicked", G_CALLBACK( publicoRegistrarPesoBruto ), nullptr ) );
}

void vistaLeerPesoTaraPublico()
{
	string pesoBruto = interfaz.obtenerTextoEtiqueta( "EntradaPesoBrutoPublico" );
	if( !registroPublico -> estaPesoBrutoEstablecido() ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampoPublico", "Es necesario registrar primero el peso bruto." );
		interfaz.mostrarElemento( "MensajeErrorCampoPublico" );
		return;
	}
	interfaz.ocultarElemento( "MensajeErrorCampoPublico" );
	lectorBascula.abrir( interfaz );
	lectorBascula.establecerIdSenal( interfaz.conectarSenal( "BotonRegistrarPeso", "clicked", G_CALLBACK( publicoRegistrarPesoTara ), nullptr ) );
}

void vistaConsultarPesajesInternos()
{
	// Muestra los registros del día
	internoObtenerPorFecha( registrosInternosConsultados, obtenerFecha() );
	internoActualizarRegistros( registrosInternosConsultados, "ContenedorRegistrosPesaje" );

	// Establece la fecha del ticket que se está consultando
	interfaz.establecerTextoEtiqueta( "TicketsRegistrados", "Registros del día " + obtenerFecha() + ":"  );

	// Establece el número de tickets
	interfaz.establecerTextoEtiqueta( "TicketsContados", to_string( registrosInternosConsultados.size() ) + " registros"  );

	// Señal de retorno consultar pesajes internos
	interfaz.desconectarSenal( "EnlaceRegistrosPesajeRegresar", enlaceRegistrosPesajeRegresarId );
	enlaceRegistrosPesajeRegresarId = interfaz.conectarSenal( "EnlaceRegistrosPesajeRegresar", "activate-link", G_CALLBACK( irHacia ), (void *)"Registros" );

	// Señal de retorno consultar dia
	interfaz.desconectarSenal( "EnlaceRegistrosPesajeRegresarConsulta", enlaceRegistrosPesajeRegresarConsultaId );
	enlaceRegistrosPesajeRegresarConsultaId = interfaz.conectarSenal( "EnlaceRegistrosPesajeRegresarConsulta", "activate-link", G_CALLBACK( irHacia ), (void *)"Pesajes" );

	// Señal del boton seleccionar día
	interfaz.desconectarSenal( "BotonRegistrosPesajeSeleccionarDia", botonRegistrosPesajeSeleccionarDiaId );
	botonRegistrosPesajeSeleccionarDiaId = interfaz.conectarSenal( "BotonRegistrosPesajeSeleccionarDia", "clicked", G_CALLBACK( irHacia ), ( void *)"ConsultarDia" );

	// Señal del boton informe
	interfaz.desconectarSenal( "BotonRegistrosPesajeObtenerInforme", botonRegistrosPesajeObtenerInformeId );
	botonRegistrosPesajeObtenerInformeId = interfaz.conectarSenal( "BotonRegistrosPesajeObtenerInforme", "clicked", G_CALLBACK( internoGenerarInforme ), nullptr );

	// Señal de obtener registros
	interfaz.desconectarSenal( "BotonObtenerRegistros", botonObtenerRegistrosId );
	botonObtenerRegistrosId = interfaz.conectarSenal( "BotonObtenerRegistros", "clicked", G_CALLBACK( irHacia ), (void *)"ObtenerRegistros" );

	// Señal de selección de ticket (entrada)
	interfaz.desconectarSenal( "EntradaRegistrosPesajeFolio", entradaRegistrosPesajeFolioId );
	entradaRegistrosPesajeFolioId = interfaz.conectarSenal( "EntradaRegistrosPesajeFolio", "activate", G_CALLBACK( vistaConsultarRegistroInterno ), nullptr );

	// Señal de selección de ticket (boton)
	interfaz.desconectarSenal( "BotonRegistrosPesajeConsultarRegistro", botonRegistrosPesajeConsultarRegistroId );
	botonRegistrosPesajeConsultarRegistroId = interfaz.conectarSenal( "BotonRegistrosPesajeConsultarRegistro", "clicked", G_CALLBACK( vistaConsultarRegistroInterno ), nullptr );

	// Señal de retorno vista generar informe
	interfaz.desconectarSenal( "EnlaceRegistrosPesajeRegresarInforme", enlaceRegistrosPesajeRegresarInformeId );
	enlaceRegistrosPesajeRegresarInformeId = interfaz.conectarSenal( "EnlaceRegistrosPesajeRegresarInforme", "activate-link", G_CALLBACK( irHacia ), (void *)"Pesajes" );

	// Boton de seleccion dia
	interfaz.desconectarSenal( "BotonConsultarDia", botonConsultarDiaId );
	botonConsultarDiaId = interfaz.conectarSenal( "BotonConsultarDia", "clicked", G_CALLBACK( internoSeleccionarDia ), nullptr );

	// Boton de generación de informe
	interfaz.desconectarSenal( "BotonObtenerRegistrosRango", botonObtenerRegistrosRangoId );
	botonObtenerRegistrosRangoId = interfaz.conectarSenal( "BotonObtenerRegistrosRango", "clicked", G_CALLBACK( internoObtenerRegistrosRango ), nullptr );

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
	interfaz.desconectarSenal( "EnlaceRegistrosPesajeRegresar", enlaceRegistrosPesajeRegresarId );
	enlaceRegistrosPesajeRegresarId = interfaz.conectarSenal( "EnlaceRegistrosPesajeRegresar", "activate-link", G_CALLBACK( irHacia ), (void *)"Registros" );

	// Señal de retorno consultar dia
	interfaz.desconectarSenal( "EnlaceRegistrosPesajeRegresarConsulta", enlaceRegistrosPesajeRegresarConsultaId );
	enlaceRegistrosPesajeRegresarConsultaId = interfaz.conectarSenal( "EnlaceRegistrosPesajeRegresarConsulta", "activate-link", G_CALLBACK( irHacia ), (void *)"Pesajes" );

	// Señal del boton seleccionar día
	interfaz.desconectarSenal( "BotonRegistrosPesajeSeleccionarDia", botonRegistrosPesajeSeleccionarDiaId );
	botonRegistrosPesajeSeleccionarDiaId = interfaz.conectarSenal( "BotonRegistrosPesajeSeleccionarDia", "clicked", G_CALLBACK( irHacia ), ( void *)"ConsultarDia" );

	// Señal del boton informe
	interfaz.desconectarSenal( "BotonRegistrosPesajeObtenerInforme", botonRegistrosPesajeObtenerInformeId );
	botonRegistrosPesajeObtenerInformeId = interfaz.conectarSenal( "BotonRegistrosPesajeObtenerInforme", "clicked", G_CALLBACK( publicoGenerarInforme ), nullptr );

	// Señal de obtener registros
	interfaz.desconectarSenal( "BotonObtenerRegistros", botonObtenerRegistrosId );
	botonObtenerRegistrosId = interfaz.conectarSenal( "BotonObtenerRegistros", "clicked", G_CALLBACK( irHacia ), (void *)"ObtenerRegistros" );	

	// Señal de selección de ticket (entrada)
	interfaz.desconectarSenal( "EntradaRegistrosPesajeFolio", entradaRegistrosPesajeFolioId );
	entradaRegistrosPesajeFolioId = interfaz.conectarSenal( "EntradaRegistrosPesajeFolio", "activate", G_CALLBACK( vistaConsultarRegistroPublico ), nullptr );

	// Señal de selección de ticket (boton)
	interfaz.desconectarSenal( "BotonRegistrosPesajeConsultarRegistro", botonRegistrosPesajeConsultarRegistroId );
	botonRegistrosPesajeConsultarRegistroId = interfaz.conectarSenal( "BotonRegistrosPesajeConsultarRegistro", "clicked", G_CALLBACK( vistaConsultarRegistroPublico ), nullptr );

	// Señal de retorno vista generar informe
	interfaz.desconectarSenal( "EnlaceRegistrosPesajeRegresarInforme", enlaceRegistrosPesajeRegresarInformeId );
	enlaceRegistrosPesajeRegresarInformeId = interfaz.conectarSenal( "EnlaceRegistrosPesajeRegresarInforme", "activate-link", G_CALLBACK( irHacia ), (void *)"Pesajes" );

	// Boton de seleccion dia
	interfaz.desconectarSenal( "BotonConsultarDia", botonConsultarDiaId );
	botonConsultarDiaId = interfaz.conectarSenal( "BotonConsultarDia", "clicked", G_CALLBACK( publicoSeleccionarDia ), nullptr );

	// Boton de generación de informe
	interfaz.desconectarSenal( "BotonObtenerRegistrosRango", botonObtenerRegistrosRangoId );
	botonObtenerRegistrosRangoId = interfaz.conectarSenal( "BotonObtenerRegistrosRango", "clicked", G_CALLBACK( publicoObtenerRegistrosRango ), nullptr );

	// Redirige hacia la vista
	irHacia( nullptr, (void *)"Pesajes" );
}

void vistaConsultarRegistroInterno()
{
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

	// Folio
	interfaz.establecerTextoEtiqueta( "FolioInterno", to_string( ticket -> obtenerFolio() ) );

	// Fecha
	interfaz.establecerTextoEtiqueta( "FechaInterno", ticket -> obtenerFecha() );

	// Empresa
	interfaz.establecerTextoEntrada( "NombreEmpresaInterno", ticket -> obtenerEmpresa() -> obtenerNombre() );

	// Producto
	interfaz.establecerTextoEntrada( "NombreProductoInterno", ticket -> obtenerProducto() -> obtenerNombre() );

	// Nombre del conductor
	interfaz.establecerTextoEntrada( "NombreConductorInterno", ticket -> obtenerNombreConductor() );

	// Número de placas
	interfaz.establecerTextoEntrada( "NumeroPlacasInterno", ticket -> obtenerNumeroPlacas() );

	// Hora entrada
	interfaz.establecerTextoEtiqueta( "HoraEntradaInterno", ticket -> obtenerHoraEntrada() );

	// Peso bruto
	interfaz.establecerTextoEtiqueta( "PesoBrutoInterno", to_string( ticket -> obtenerPesoBruto() ) );

	// Hora salida
	interfaz.establecerTextoEtiqueta( "HoraSalidaInterno", ticket -> obtenerHoraSalida() );

	// Peso tara
	interfaz.establecerTextoEtiqueta( "PesoTaraInterno", to_string( ticket -> obtenerPesoTara() ) ) ;


	// Descuento (permiso)
	interfaz.establecerTextoEntrada( "PermitirDescuentoInterno", ( ticket -> permitirDescuento() ? "Sí" : "No" ) );

	// Descuento
	interfaz.establecerTextoEntrada( "DescuentoInterno", ( ticket -> permitirDescuento() ? to_string( ticket -> obtenerDescuento() ) : "" ) );

	// Tipo de registro
	interfaz.establecerTextoEntrada( "TipoRegistroInterno", ( ticket -> obtenerTipoRegistro() == TIPO_REGISTRO_ENTRADA ? "Entrada" : "Salida" ) );

	// Peso neto
	interfaz.establecerTextoEtiqueta( "PesoNetoInterno", to_string( ticket -> obtenerPesoNeto() ) );

	// Observaciones
	interfaz.establecerTextoEntrada( "ObservacionesInterno", ticket -> obtenerObservaciones() );

	// Señal de boton si
	interfaz.desconectarSenal( "BotonSi", botonSiId );
	botonSiId = interfaz.conectarSenal( "BotonSi", "clicked", G_CALLBACK( internoEliminarSeleccionado ), nullptr );

	// Señal boton imprimir interno
	interfaz.desconectarSenal( "ImprimirRegistroInterno", imprimirRegistroInternoId );
	imprimirRegistroInternoId = interfaz.conectarSenal( "ImprimirRegistroInterno", "clicked", G_CALLBACK( internoImprimirSeleccionado ), nullptr );

	// Señal boton de eliminar registro interno seleccionado
	interfaz.desconectarSenal( "EliminarRegistroInterno", eliminarRegistroInternoId );
	eliminarRegistroInternoId = interfaz.conectarSenal( "EliminarRegistroInterno", "clicked", G_CALLBACK( internoAlertaEliminar ), nullptr );

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

	// Folio
	interfaz.establecerTextoEtiqueta( "FolioPublico", to_string( registroPublico -> obtenerFolio() ) );

	// Fecha
	interfaz.establecerTextoEtiqueta( "FechaPublico", registroPublico -> obtenerFecha() );

	// Producto
	interfaz.establecerTextoEntrada( "NombreProductoPublico", registroPublico -> obtenerProducto() -> obtenerNombre() );

	// Nombre del conductor
	interfaz.establecerTextoEntrada( "NombreConductorPublico", registroPublico -> obtenerNombreConductor() );

	// Número de placas
	interfaz.establecerTextoEntrada( "NumeroPlacasPublico", registroPublico -> obtenerNumeroPlacas() );

	// Hora entrada
	interfaz.establecerTextoEtiqueta( "HoraEntradaPublico", registroPublico -> obtenerHoraEntrada() );

	// Peso bruto
	interfaz.establecerTextoEtiqueta( "PesoBrutoPublico", to_string( registroPublico -> obtenerPesoBruto() ) );

	// Hora salida
	interfaz.establecerTextoEtiqueta( "HoraSalidaPublico", registroPublico -> obtenerHoraSalida() );

	// Peso tara
	interfaz.establecerTextoEtiqueta( "PesoTaraPublico", to_string( registroPublico -> obtenerPesoTara() ) ) ;

	// Tipo de registro
	interfaz.establecerTextoEntrada( "TipoViajePublico", ( registroPublico -> obtenerTipoViaje() == VIAJE_LOCAL ? "Local" : "Foráneo" ) );

	// Peso neto
	interfaz.establecerTextoEtiqueta( "PesoNetoPublico", to_string( registroPublico -> obtenerPesoNeto() ) );

	// Señal boton de eliminar registro interno seleccionado
	interfaz.desconectarSenal( "EliminarRegistroInterno", eliminarRegistroInternoId );
	eliminarRegistroInternoId = interfaz.conectarSenal( "EliminarRegistroInterno", "clicked", G_CALLBACK( internoAlertaEliminar ), nullptr );

	// Señal de boton si
	interfaz.desconectarSenal( "BotonSi", botonSiId );
	botonSiId = interfaz.conectarSenal( "BotonSi", "clicked", G_CALLBACK( publicoEliminarSeleccionado ), nullptr );

	// Señal boton imprimir interno
	interfaz.desconectarSenal( "ImprimirRegistroPublico", imprimirRegistroInternoId );
	imprimirRegistroInternoId = interfaz.conectarSenal( "ImprimirRegistroPublico", "clicked", G_CALLBACK( publicoImprimirSeleccionado ), nullptr );

	// Redirige a la vista
	irHacia( nullptr, (void *)"RegistroPublico" );
}

void vistaConsultarUsuarios()
{
	// 
	obtenerUsuariosRegistrados();

	//
	actualizarRegistrosUsuarios( registrosUsuarios, "ContenedorUsuarios" );

	// 
	interfaz.desconectarSenal( "EntradaConsultarUsuarios", entradaConsultarUsuarioId );
	entradaConsultarUsuarioId = interfaz.conectarSenal( "EntradaConsultarUsuario", "activate", G_CALLBACK( vistaConsultarUsuario ), nullptr );

	//
	interfaz.desconectarSenal( "BotonConsultarUsuarios", botonConsultarUsuarioId );
	botonConsultarUsuarioId = interfaz.conectarSenal( "BotonConsultarUsuario", "clicked", G_CALLBACK( vistaConsultarUsuario ), nullptr );

	//
	interfaz.desconectarSenal( "EnlaceConsultarUsuariosRegresar", enlaceConsultarUsuariosRegresarId );
	enlaceConsultarUsuariosRegresarId = interfaz.conectarSenal( "EnlaceConsultarUsuariosRegresar", "activate-link", G_CALLBACK( regresarInicio ), nullptr );

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

	// Señal Check button
	interfaz.desconectarSenal( "UsuarioAdministrador", usuarioAdministradorId  );
	usuarioAdministradorId = interfaz.conectarSenal( "UsuarioAdministrador", "toggled", G_CALLBACK( actualizarEstadoAdministrador ), nullptr );

	//Señal Eliminar usuarios	
	interfaz.desconectarSenal( "BotonEliminarUsuario", botonEliminarUsuarioId );
	botonEliminarUsuarioId = interfaz.conectarSenal( "BotonEliminarUsuario", "clicked", G_CALLBACK( alerta ), (void *)"¿Estás seguro que deseas eliminar este usuartio?" );

	// Señal Recuperar contraseña
	interfaz.desconectarSenal( "BotonObtenerCodigoRecuperacion", botonObtenerCodigoRecuperacionId );	
	botonObtenerCodigoRecuperacionId = interfaz.conectarSenal( "BotonObtenerCodigoRecuperacion", "clicked", G_CALLBACK( generarCodigoRecuperacion ), nullptr );

	// Boton si si decide eliminar
	interfaz.desconectarSenal( "BotonSi", botonSiId );
	botonSiId = interfaz.conectarSenal( "BotonSi", "clicked", G_CALLBACK( eliminarUsuario ), nullptr );

	// Señal regresar
	interfaz.desconectarSenal( "EnlaceConsultarUsuarioRegresar", enlaceConsultarUsuarioRegresarId );
	enlaceConsultarUsuarioRegresarId = interfaz.conectarSenal( "EnlaceConsultarUsuarioRegresar", "activate-link", G_CALLBACK( regresarUsuarios ), nullptr );

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
	interfaz.desconectarSenal( "BotonCambiarContrasena", botonCambiarContrasenaId );
	botonCambiarContrasenaId = interfaz.conectarSenal( "BotonCambiarContrasena", "clicked", G_CALLBACK( cambiarContrasenaUsuario ), nullptr );

	// Boton de confirmacion
	interfaz.desconectarSenal( "EntradaReemplazarConfirmacion", entradaReemplazarConfirmacionId );
	entradaReemplazarConfirmacionId = interfaz.conectarSenal( "EntradaReemplazarConfirmacion", "activate", G_CALLBACK( cambiarContrasenaUsuario ), nullptr );

	// Redirige hacia la vista
	irHacia( nullptr, (void *)"ReemplazarContrasena" );
}
