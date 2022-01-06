#include "Vistas.h"
#include "Aplicacion.h"
#include "Widget.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <list>
#include "Funciones.h"
#include "Sesion.h"
#include "Imagen.h"
#include "GestorBasculas.h"
#include "GestorRegistros.h"
#include "RegistrosInternos.h"
#include "RegistrosPublicos.h"
#include "Senales.h"
using namespace std;

void vistaCuenta( GtkWidget *widget, gpointer ptr )
{
	// Establece las opciones para cuenta
	gtk_entry_set_text( GTK_ENTRY( "EntradaCuentaNombre" ), usuario.obtenerNombre().c_str() );
	gtk_entry_set_text( GTK_ENTRY( "EntradaCuentaApellidos" ), usuario.obtenerApellidos().c_str() );
	gtk_entry_set_text( GTK_ENTRY( "EntradaCuentaNombreUsuario" ), usuario.obtenerNombreUsuario().c_str() );
	gtk_entry_set_text( GTK_ENTRY( "EntradaCuentaContrasenaNueva" ), "" );
	gtk_entry_set_text( GTK_ENTRY( "EntradaCuentaContrasenaConfirmacion" ), "" );

	// Se dirige a cuenta
	irA( "Cuenta", false );
}

void vista_registros( GtkWidget *widget, gpointer ptr ){	
	// Lo convierte para establecer el título
	ContenedorRegistros *registros = static_cast< ContenedorRegistros * >( ptr );
	string nombre = registros -> obtenerNombreSingular();
	nombre[ 0 ] = toupper( nombre[ 0 ] );

	// Establece la vista para consultar registros
	irA( "ConsultarRegistros", false );

	// Establece el nombre de los registros que se estan consultando
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "Titulo" ) ), nombre.c_str() );
	
	// Establece la imagen de registro
	string ruta = "../recursos/imagenes/iconos/";
	gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenRegistro" ) ), (ruta + nombre + ".png").c_str() );
	gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenEdicionRegistro" ) ), "../recursos/imagenes/iconos/Producto.png" );
	
	// Actualiza la lista de registros de empresas
	registros -> actualizarListaRegistros();
	
	// Vistas  
	conectar_senal( botonRegistroVistaConsultar, ptr );
	conectar_senal( botonRegistroVistaNuevo, (void *)"NuevoRegistro" );
	conectar_senal( botonRegistroVistaEditar, ptr );
	conectar_senal( botonRegistroVistaEliminar, nullptr );

	// Acciones
	conectar_senal( botonRegistroGuardarNuevo, ptr );
	conectar_senal( botonRegistroGuardarEdicion, ptr );
    conectar_senal( botonSi, ptr );
}

void vistaRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data ){
	// Obtiene el id del folio
	unsigned int folio = obtenerFolioSelector( row );
	
	// Obtiene el folio
	string registro = (char *)data;
	database.open( databaseFile );
	database.query( "select * from " + registro + " where id_producto = " + to_string( folio ) );
	database.close();

	if( results.size() > 0 ){
		// Obtiene el renglon
		unordered_map< string, string > *resultados = results.at( 0 );

		// Establece la clave del registro
		stringstream clave;
		clave << setfill( '0' ) << setw( 7 ) << folio << setfill( ' ' );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaClaveRegistro" ) ), clave.str().c_str() );
	
		// Establece el nombre del registro
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaNombreRegistro" ) ), (* resultados)[ "nombre" ].c_str() );

		// Establece la descripción
		if( (* resultados)[ "descripcion" ].compare( "null" ) == 0 ){
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaDescripcionRegistro" ) ), "Ninguno." );
		}
		else{
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaDescripcionRegistro" ) ), (* resultados)[ "descripcion" ].c_str() );
		}

		// Establece imagen
		if( (* resultados)[ "imagen" ].compare( "null" ) == 0 ){
			gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenRegistroConsulta" ) ), "../recursos/imagenes/iconos/Producto.png" );
		}
		else{
			string ruta = "../recursos/imagenes/";
			ruta = ruta + (char *)data + "s/" + (* resultados)[ "imagen" ];
			GdkPixbuf *imagen = imagen_cargar_escalar( ruta.c_str(), 180, 180 );
			gtk_image_set_from_pixbuf( GTK_IMAGE(  buscar_objeto( "ImagenRegistroConsulta" ) ), imagen );
		}

		// Establece la vista de registro
		irA( "Registro", false );
	}
}

void vistaRegistroEditar( GtkWidget *widget, gpointer data )
{
	try{
        GtkListBoxRow *itemSeleccionado = gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistros" ) ) );
        if( itemSeleccionado == NULL ){
            app_mostrar_error( "No se ha seleccionado ningún registro." );
            return;
        }

		stringstream consulta;
		consulta << "select * from Producto where id_producto = "
				 << gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( itemSeleccionado ) ) ) );
		
		// Realiza la consultas
		database.open( databaseFile );
		database.query( consulta.str() );
		database.close();

		if( results.size() > 0 ){
			unordered_map< string, string > *resultado = results.at( 0 );

			// Estabece los datos del formulario
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaClaveEditarRegistro" ) ), (* resultado )[ "id_producto" ].c_str() );
			gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreEditarRegistro" ) ), (* resultado )[ "nombre" ].c_str() );
			if( (* resultado)[ "descripcion" ].compare( "null" ) != 0 ){
				gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "ProductoDescripcionEditar" ) ), (* resultado )[ "descripcion" ].c_str() );
			}
		}

		irA( "ProductoEditar", false );
	}
	catch( invalid_argument &ia ){
		app_mostrar_error( ia.what() );
	}
}

void vistaRegistroEliminar( GtkWidget *widget, gpointer ptr )
{
	const char *mensaje = "ALERTA: Si el existen registros de pesaje que hagan\nreferencia a este registro, también serán eliminados.\n¿Estás seguro que deseas hacerlo?";
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "MensajeAlerta" ) ), mensaje );
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
	interfaz.establecerTextoEtiqueta( "EntradaFechaPublico", tiempo_leer_fecha_corta() );
	
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
		app_mostrar_mensaje( "Introduzca un folio válido." );
	} 
}

void vistaCrearRegistro( GtkWidget *widget, gpointer ptr )
{	
	// Folio
	stringstream folio;
	folio << setfill( '0' ) << setw( 7 ) << (folioActual + 1);
	interfaz.establecerTextoEtiqueta( "EntradaFolioInterno", folio.str() );
	
	// Fecha
	interfaz.establecerTextoEtiqueta( "EntradaFechaInterno", tiempo_leer_fecha_corta() );
	
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
	internoObtenerPorFecha( registrosInternosConsultados, tiempo_leer_fecha_corta() );
	internoActualizarRegistros( registrosInternosConsultados, "ContenedorRegistrosPesaje" );

	// Establece la fecha del ticket que se está consultando
	interfaz.establecerTextoEtiqueta( "TicketsRegistrados", "Registros del día " + tiempo_leer_fecha_corta() + ":"  );

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
	publicoObtenerPorFecha( registrosPublicosConsultados, tiempo_leer_fecha_corta() );
	publicoActualizarRegistros( registrosPublicosConsultados, "ContenedorRegistrosPesaje" );

	// Establece la fecha del ticket que se está consultando
	interfaz.establecerTextoEtiqueta( "TicketsRegistrados", "Registros del día " + tiempo_leer_fecha_corta() + ":"  );

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
		app_mostrar_mensaje( "Registro no encontrado." );
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
		app_mostrar_mensaje( "Registro no encontrado o no disponible." );
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
		app_mostrar_mensaje( "El usuario que se desea consultar no fue encontrado." );
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
	interfaz.conectarSenal( botonEliminarUsuario, G_CALLBACK( app_alerta ), (void *)"¿Estás seguro que deseas eliminar este usuario?" );

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
