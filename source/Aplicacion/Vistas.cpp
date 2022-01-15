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
	conectar_senal( botonRegistroVistaConsultar, G_CALLBACK( crearRegistro ), ptr );
	conectar_senal( botonRegistroVistaNuevo, G_CALLBACK( irHacia ), (void *)"NuevoRegistro" );
	conectar_senal( botonRegistroVistaEditar, G_CALLBACK( vistaRegistroEditar ), (void *)ptr );
	conectar_senal( botonRegistroVistaEliminar, G_CALLBACK( vistaRegistroEliminar ), nullptr );

	// Acciones
	//conectar_senal( botonRegistroGuardarNuevo, ptr );
	conectar_senal( botonRegistroGuardarEdicion, G_CALLBACK( actualizarRegistro ), ptr );
    conectar_senal( botonSi, G_CALLBACK( eliminarRegistro ), ptr );
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

void vistaRegistroEliminar( GtkWidget *widget, gpointer ptr ){
	const char *mensaje = "ALERTA: Si el existen registros de pesaje que hagan\nreferencia a este registro, también serán eliminados.\n¿Estás seguro que deseas hacerlo?";
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "MensajeAlerta" ) ), mensaje );
	gtk_widget_show( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );
}

void vistaBasculaEliminar(){
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "MensajeAlerta" ) ), "¿Quieres eliminar la báscula seleccionada?" );
	gtk_widget_show( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );
}

void vistaBasculaEdicion(){
	basculaLimpiarFormulario();

	irA( "OpcionesBascula", false );
}


void vistaConfiguracion( GtkWidget *widget, gpointer ptr )
{
	// Obtiene la información de la empresa
	database.open( databaseFile );
	database.query( "select * from EmpresaPropia where id_empresa = 1" );
	database.close();

	if( results.size() > 0 ){
		unordered_map< string, string > *resultado = results.at( 0 );

		// Actualiza el nombre
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreEmpresa" ) ), (* resultado)[ "razon_social" ].c_str() );

		// Actualiza el RFC si tiene
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EmpresaPropiaRFC" ) ), "No establecido" );
		if( (* resultado)[ "RFC" ].compare( "null" ) != 0 ){
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EmpresaPropiaRFC" ) ), (* resultado)[ "RFC" ].c_str() );
		}

		// Obtiene la imagen de la empresa
		gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenEmpresaPropia" ) ), "../recursos/imagenes/iconos/Empresas.png" );
		if( (* resultado)[ "imagen" ].compare( "null" ) != 0 ){
			GdkPixbuf *imagen = imagen_cargar_escalar( "../recursos/imagenes/empresas/" + (*resultado)[ "imagen" ], 128, 128 );
            if( imagen != nullptr ){
            	gtk_image_set_from_pixbuf( GTK_IMAGE( gtk_builder_get_object( builder, "ImagenEmpresaPropia" ) ), imagen );
            }
		}
	} 

	// Establece la señal de eliminar bascula
	//conectar_senal( botonSi, G_CALLBACK( basculaEliminar ), nullptr );

	// Se dirige a la vista
	irA( "Configuracion", false );
}

void vistaBasculaPublica( GtkWidget *widget, gpointer ptr )
{	
	// Obtiene los tickets pendientes
	publicoActualizarRegistros( registrosPublicosPendientes, "ContenedorTickets" );
	
	// Conecta las señales correspondientes
	conectar_senal( botonBasculaNuevo, G_CALLBACK( vistaCrearRegistroPublico ), nullptr );
	conectar_senal( ticketsPendientesSeleccionados, G_CALLBACK( vistaPublicoEditarRegistro ), nullptr );

	// Establece las vistas
	irA( "Tickets", false );
}

void vistaBasculaInterna( GtkWidget *widget, gpointer ptr )
{	
	// Obtiene los tickets pendiente
	internoActualizarRegistros( registrosInternosPendientes, "ContenedorTickets" );
	
	// Conecta las señales correspondientes
    conectar_senal( botonBasculaNuevo, G_CALLBACK( vistaCrearRegistro ), nullptr );
    conectar_senal( ticketsPendientesSeleccionados, G_CALLBACK( vistaInternoEditarRegistro ), nullptr);

    // Establece las vistas
	irHacia( nullptr, (void *)"Tickets" );
}

void vistaCrearRegistroPublico( GtkWidget *widget, gpointer ptr )
{	
	// Folio
	stringstream folio;
	folio << setfill( '0' ) << setw( 7 ) << (folioActualPublico + 1);
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ), folio.str() );
	
	// Fecha
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFechaPublico", tiempo_leer_fecha_corta() );
	
	// Producto
	gtk_entry_set_editable( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico" ) ), TRUE );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico" ) ), "" );
	
	// Nombre del conductor
	gtk_entry_set_editable( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorPublico"  ) ), TRUE );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorPublico" ) ), "" );
	
	// Numero de placas
	gtk_entry_set_editable( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasPublico"  ) ), TRUE );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasPublico" ), "" ) );
	
	// Hora entrada y Peso Bruto
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaPublico" ) ), "No establecida" );
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoPublico" ) ), "No establecido" );
	
	// Hora salida y Peso Tara
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaPublico" ) ), "No establecida" );
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ), "No establecido" );
	
	// Tipo de registro
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( buscar_objeto( "ViajeLocal" ) ), TRUE );
	
	// Peso neto
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoPublico" ) ), "No establecido" ) );
	
	// Establece el completador de producto
	//  gtk_entry_set_completion( GTK_ENTRY( objecto ), completador );
	//establecerCompletadorEntrada( "EntradaNombreProductoInterno", NULL );
	//establecerCompletadorEntrada( "EntradaNombreProductoPublico", productos.obtenerCompletador() ); 
	
	// Establece la vista de nuevo ticket
	irA( "NuevoTicketPublico", false );
}

void vistaPublicoEditarRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data )
{
	try{
		// Busca el ticket
		TicketPublico *registroPublico = buscarRegistroPublicoPorFolio( obtenerFolioSelector( row ), registrosPublicosPendientes );
		if( registroPublico == nullptr ){
			app_mostrar_mensaje( "Registro no encontrado." );
			return;
		}
		
		// Establece los datos de la vista
		stringstream entrada;
		
		// Folio
		entrada << setfill( '0' ) << setw( 7 ) << registroPublico -> obtenerFolio();
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ), entrada.str().c_str() ) );
		
		// Fecha
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFechaPublico" ) ), registroPublico -> obtenerFecha().c_str() );
		
		// Tipo registro
		if( registroPublico -> obtenerTipoViaje() == VIAJE_LOCAL ){
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( buscar_objeto( "ViajeLocal" ) ),TRUE );
		}
		else{
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( buscar_objeto( "ViajeForaneo" ) ),TRUE );
		}
		
		// Nombre del producto (no permite edicion)
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico" ) ), registroPublico -> obtenerProducto() -> obtenerNombre() ) );
		gtk_entry_set_editable( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico"  ) ), FALSE );
		
		// Nombre del conductor (no permite edicion)
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorPublico" ) ), registroPublico -> obtenerNombreConductor() ) );
		gtk_entry_set_editable( GTK_ENTRY( buscar_objeto(  "EntradaNombreConductorPublico"  ) ), FALSE );
		
		// Numero de placas (no permite edicion)
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasPublico" ) ), registroPublico -> obtenerNumeroPlacas() );
		gtk_entry_set_editable( GTK_ENTRY( buscar_objeto(  "EntradaNumeroPlacasPublico"  ) ), FALSE );
		
		// Hora entrada y peso bruto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaPublico" ) ), ( registroPublico -> estaPesoBrutoEstablecido() ? registroPublico -> obtenerHoraEntrada() : "No establecida" ) );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoPublico" ) ), ( registroPublico -> estaPesoBrutoEstablecido() ? pesoString( registroPublico -> obtenerPesoBruto(), 2 ) : "No establecido" ) );
		
		// Hora salida y peso tara
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaPublico" ) ), ( registroPublico -> estaPesoTaraEstablecido() ? registroPublico -> obtenerHoraSalida() : "No establecida" ) );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ), ( registroPublico -> estaPesoTaraEstablecido() ? pesoString( registroPublico -> obtenerPesoTara(), 2 ) : "No establecido" ) );
		
		// Peso neto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoPublico" ) ), ( registroPublico -> estaPesoNetoEstablecido() ? pesoString( registroPublico -> obtenerPesoNeto(), 2 ) : "No establecido" ) );
		
		// Establece el texto de finalizar
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "BotonRegistrarPublico" ) ), "Finalizar" );

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
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ), folio.str().C_str() );
	
	// Fecha
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFechaInterno" ) ), tiempo_leer_fecha_corta().c_str() );
	
	// Limpia el formulario
	internoLimpiarFormulario();

	// Descuento
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( buscar_objeto( "NoDescuentoInterno" ) ), TRUE );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ), "" );
	gtk_entry_set_editable( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ), FALSE );

	// Tipo de registro (registra entrada por defecto)
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( buscar_objeto( "RegistraEntrada" ) ), TRUE );

	// Establece el completador de empresa
	//establecerCompletadorEntrada( "EntradaNombreEmpresaInterno", empresas.obtenerCompletador() );
	
	// Establece el completador de producto
	//establecerCompletadorEntrada( "EntradaNombreProductoPublico", NULL );
	//establecerCompletadorEntrada( "EntradaNombreProductoInterno", productos.obtenerCompletador() );
	
	// Establece la vista de nuevo ticket 
	irHacia( nullptr, (void *)"NuevoTicketInterno" );
}

void vistaInternoEditarRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data )
{
	Ticket *ticket = buscarRegistroInternoPorFolio( obtenerFolioSelector( row ), registrosInternosPendientes );
	
	// Si no encuentra el ticket
	if( ticket == nullptr ){
		app_mostrar_mensaje( "Registro no encontrado." );
		return;
	}
	
	// Establece los datos de la vista
	stringstream entrada;
	
	// Folio
	entrada << setfill( '0' ) << setw( 7 ) << ticket -> obtenerFolio();
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ), entrada.str().c_str() );
	
	// Fecha
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFechaInterno" ) ), ticket -> obtenerFecha().c_str() );
	
	// Tipo registro
	if( ticket -> obtenerTipoRegistro() == TIPO_REGISTRO_ENTRADA ){
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( buscar_objeto( "RegistraEntrada" ) ), TRUE );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaEmpresa" ) ), "Proveedor:      " );
	}
	else{
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( buscar_objeto( "RegistraSalida" ) ), TRUE );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaEmpresa" ) ), "Cliente:           " );
	}

	// Nombre de la empresa
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreEmpresaInterno" ) ), ticket -> obtenerEmpresa() -> obtenerNombre().c_str() );
	
	// Nombre del producto
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoInterno" ) ), ticket -> obtenerProducto() -> obtenerNombre().c_str() );
	
	// Nombre del conductor
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorInterno" ) ), ticket -> obtenerNombreConductor().c_str() );
	
	// Numero de placas
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasInterno" ) ), ticket -> obtenerNumeroPlacas().c_str() );
	
	// Hora entrada y peso bruto
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaInterno" ) ), ( ticket -> estaPesoBrutoEstablecido() ? ticket -> obtenerHoraEntrada() : "No establecida" ).c_str() );
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoInterno" ) ), ( ticket -> estaPesoBrutoEstablecido() ? pesoString( ticket -> obtenerPesoBruto(), 2 ) : "No establecido" ).c_str() );
	
	// Hora salida y peso tara
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaInterno" ) ), ( ticket -> estaPesoTaraEstablecido() ? ticket -> obtenerHoraSalida() : "No establecida" ).c_str() );
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ), ( ticket -> estaPesoTaraEstablecido() ? pesoString( ticket -> obtenerPesoTara(), 2 ) : "No establecido" ).c_str() );
	
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
	conectar_senal( botonRegistrarPeso, G_CALLBACK( internoRegistrarPesoBruto ), nullptr );
}

void vistaLeerPesoTara()
{
	basculaAbrirLector();
	conectar_senal( botonRegistrarPeso, G_CALLBACK( internoRegistrarPesoTara ), nullptr );
}

void vistaLeerPesoBrutoPublico()
{
	basculaAbrirLector();
	conectar_senal( botonRegistrarPeso, G_CALLBACK( publicoRegistrarPesoBruto ), nullptr );
}

void vistaLeerPesoTaraPublico()
{
	basculaAbrirLector();
	conectar_senal( botonRegistrarPeso, G_CALLBACK( publicoRegistrarPesoTara ), nullptr );
}

void vistaConsultarPesajesInternos()
{
	// Muestra los registros del día
	internoObtenerPorFecha( registrosInternosConsultados, tiempo_leer_fecha_corta() );
	internoActualizarRegistros( registrosInternosConsultados, "ContenedorRegistrosPesaje" );

	// Establece la fecha del ticket que se está consultando
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "TicketsRegistrados" ) ), ("Registros del día " + tiempo_leer_fecha_corta() + ":").c_str() );

	// Establece el número de tickets
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "TicketsContados" ) ), (to_string( registrosInternosConsultados.size() ) + " registros").c_str() );

	// Señal del boton seleccionar día
	conectar_senal( botonRegistrosPesajeSeleccionarDia, G_CALLBACK( irHacia ), ( void *)"ConsultarDia" );

	// Señal del boton informe
	conectar_senal( botonRegistrosPesajeObtenerInforme, G_CALLBACK( internoGenerarInforme ), nullptr );

	// Señal de obtener registros
	conectar_senal( botonObtenerRegistros, G_CALLBACK( irHacia ), (void *)"ObtenerRegistros" );

	// Señal de selección de ticket
	conectar_senal( ticketsConsultadosSeleccionados, G_CALLBACK( vistaInternoConsultarRegistro ), nullptr );

	// Boton de seleccion dia
	conectar_senal( botonConsultarDia, G_CALLBACK( internoSeleccionarDia ), nullptr );

	// Boton de generación de informe
	conectar_senal( botonObtenerRegistrosRango, G_CALLBACK( internoObtenerRegistrosRango ), nullptr );

	// Redirige hacia la vista
	irHacia( nullptr, (void *)"Pesajes" );
}

void vistaConsultarPesajesPublicos()
{
	// Muestra los registros del día
	publicoObtenerPorFecha( registrosPublicosConsultados, tiempo_leer_fecha_corta() );
	publicoActualizarRegistros( registrosPublicosConsultados, "ContenedorRegistrosPesaje" );

	// Establece la fecha del ticket que se está consultando
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "TicketsRegistrados" ) ), ("Registros del día " + tiempo_leer_fecha_corta() + ":").c_str() );

	// Establece el número de tickets
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "TicketsContados" ) ), (to_string( registrosPublicosConsultados.size() ) + " registros").c_str() );

	// Señal del boton seleccionar día
	conectar_senal( botonRegistrosPesajeSeleccionarDia, G_CALLBACK( irHacia ), ( void *)"ConsultarDia" );

	// Señal del boton informe
	conectar_senal( botonRegistrosPesajeObtenerInforme, G_CALLBACK( publicoGenerarInforme ), nullptr );

	// Señal de obtener registros
	conectar_senal( botonObtenerRegistros, G_CALLBACK( irHacia ), (void *)"ObtenerRegistros" );	

	// Señal de selección de ticket
	conectar_senal( ticketsConsultadosSeleccionados, G_CALLBACK( vistaPublicoConsultarRegistro ), nullptr );

	// Boton de seleccion dia
	conectar_senal( botonConsultarDia, G_CALLBACK( publicoSeleccionarDia ), nullptr );

	// Boton de generación de informe
	conectar_senal( botonObtenerRegistrosRango, G_CALLBACK( publicoObtenerRegistrosRango ), nullptr );

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
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "FolioInterno" ) ), to_string( ticket -> obtenerFolio() ).c_str() ); 			 // Folio
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "FechaInterno" ) ), ticket -> obtenerFecha().c_str() );			   			 // Fecha
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "NombreEmpresaInterno" ) ), ticket -> obtenerEmpresa() -> obtenerNombre() );   // Empresa
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "NombreProductoInterno" ) ), ticket -> obtenerProducto() -> obtenerNombre() ); // Producto
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "NombreConductorInterno" ) ), ticket -> obtenerNombreConductor() );  		     // Nombre del conductor
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "NumeroPlacasInterno" ) ), ticket -> obtenerNumeroPlacas() );					// Número de placas
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "HoraEntradaInterno" ) ), ticket -> obtenerHoraEntrada()  );  				// Hora entrada
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoBrutoInterno" ) ), to_string( ticket -> obtenerPesoBruto() ) ); 			// Peso bruto
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "HoraSalidaInterno" ) ), ticket -> obtenerHoraSalida() );						// Hora salida
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoTaraInterno" ) ), to_string( ticket -> obtenerPesoTara() ) ); 			// Peso tara
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "PermitirDescuentoInterno" ) ), ( ticket -> permitirDescuento() ? "Sí" : "No" ) ); 						    	// Descuento (permiso)
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DescuentoInterno" ) ), ( ticket -> permitirDescuento() ? to_string( ticket -> obtenerDescuento() ) : "" ) );     // Descuento
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "TipoRegistroInterno" ) ), ( ticket -> obtenerTipoRegistro() == TIPO_REGISTRO_ENTRADA ? "Entrada" : "Salida" ) );	// Tipo de registro
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoNetoInterno" ) ), to_string( ticket -> obtenerPesoNeto() ) ); 			// Peso neto
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "ObservacionesInterno" ) ), ticket -> obtenerObservaciones() );				// Observaciones

	// Señal de boton si
	conectar_senal( botonSi, G_CALLBACK( internoEliminarSeleccionado ), nullptr );

	// Señal boton de eliminar registro interno seleccionado
	conectar_senal( eliminarRegistroInterno, G_CALLBACK( internoAlertaEliminar ), nullptr );

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
	
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "FolioPublico" ) ), to_string( registroPublico -> obtenerFolio() ) );						// Folio
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "FechaPublico" ) ), registroPublico -> obtenerFecha() );									// Fecha
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "NombreProductoPublico" ) ), registroPublico -> obtenerProducto() -> obtenerNombre() );	// Producto
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "NombreConductorPublico" ) ), registroPublico -> obtenerNombreConductor() );				// Nombre del conductor	
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "NumeroPlacasPublico" ) ), registroPublico -> obtenerNumeroPlacas() );					// Número de placas
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "HoraEntradaPublico" ) ), registroPublico -> obtenerHoraEntrada() );						// Hora entrada
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoBrutoPublico" ) ),  to_string( registroPublico -> obtenerPesoBruto() ) );			// Peso bruto
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "HoraSalidaPublico" ) ), registroPublico -> obtenerHoraSalida() );						// Hora salida
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoTaraPublico" ) ), to_string( registroPublico -> obtenerPesoTara() ) ) ;				// Peso tara
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "TipoViajePublico" ) ), ( registroPublico -> obtenerTipoViaje() == VIAJE_LOCAL ? "Local" : "Foráneo" ) ); // Tipo de registro
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoNetoPublico" ) ), to_string( registroPublico -> obtenerPesoNeto() );					// Peso neto

	// Señal boton de eliminar registro interno seleccionado
	conectar_senal( eliminarRegistroInterno, G_CALLBACK( internoAlertaEliminar ), nullptr );

	// Señal de boton si
	conectar_senal( botonSi, G_CALLBACK( publicoEliminarSeleccionado ), nullptr );

	// Señal boton imprimir interno
	conectar_senal( imprimirRegistroPublico, G_CALLBACK( publicoImprimirSeleccionado ), nullptr );

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
	usuarioConsultado = buscarUsuarioPorNombreUsuario( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaConsultarUsuario" ) ), registrosUsuarios ) );
	if( usuarioConsultado == nullptr ){
		app_mostrar_mensaje( "El usuario que se desea consultar no fue encontrado." );
		return;
	}

	// Nombre
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "NombreConsultado" ) ), usuarioConsultado -> obtenerNombre() );

	// Apellidos
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "ApellidosConsultado" ) ), usuarioConsultado -> obtenerApellidos() );

	// Nombre de usuario
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "NombreUsuarioConsultado" ) ), usuarioConsultado -> obtenerNombreUsuario() );

	// Administrador
	if( usuarioConsultado -> esAdministrador() ){
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( buscar_objeto( "UsuarioAdministrador") ), TRUE );
	}

	//Señal Eliminar usuarios	
	conectar_senal( botonEliminarUsuario, G_CALLBACK( app_alerta ), (void *)"¿Estás seguro que deseas eliminar este usuario?" );

	// Boton si si decide eliminar
	conectar_senal( botonSi, G_CALLBACK( eliminarUsuario ), nullptr );

	irHacia( nullptr, (void *)"InformacionUsuario" );
}

void regresarUsuarios()
{
	// No se puede acceder a la información del usuario fuera de esa vista
	usuarioConsultado = nullptr;

	// Regresa a la vista de inicio
	irHacia( nullptr, (void *)"ConsultarUsuarios" );
}

void internoLimpiarFormulario(){
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreEmpresaInterno" ) ), "" );				// Empresa
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoInterno" ) ), "" );				// Producto
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorInterno" ) ), "" );			// Nombre del conductor
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasInterno" ) ), "" );				// Numero de placas
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaInterno" ) ), "No establecida" );	// Hora entrada
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoInterno" ) ), "No establecido" );	// Peso bruto	
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaInterno" ) ), "No establecida" );	// Hora salida
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ), "No establecido" );		// Peso tara
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno" ) ), "No establecido" );		// Peso neto
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaObservacionesInterno" ) ), "" );				// Observaciones		
}

void vistaBascula( GtkListBox *box, GtkListBoxRow *row, gpointer data )
{
	// Obtiene la bascula seleccionada
	Bascula *bascula = basculaBuscarPorCodigo( obtenerFolioSelector( row ) );
	if( bascula == nullptr ){
		throw runtime_error( "Ocurrió un error al intentar recuperar la bascula seleccionada." );
	}

	// Establece las opciones que indica la bascula
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "OpcionesBasculaCodigo" ) ), to_string( obtenerFolioSelector( row ) ) );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaNombre" ) ), bascula -> obtenerNombre() );
	gtk_combo_box_set_active( GTK_COMBO_BOX( buscar_objeto( "OpcionesBasculaPuerto" ) ), bascula -> obtenerPuerto() );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBitsDatos" ) ), to_string( bascula -> obtenerByteSize() ) );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBitsStop" ) ), to_string( bascula -> obtenerStopBits() ) );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "OpcionesBasculaBytesIgnorados" ) ), to_string( bascula -> obtenerBytesIgnorados() ) );

	// Redirige hacia la vista
	irA( "OpcionesBascula", false );
}
