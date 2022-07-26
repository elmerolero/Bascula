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
#include "GestorRegistros.h"
#include "Basculas.h"
#include "PesajesInternos.h"
#include "PesajesPublicos.h"
#include "Domicilio.h"
#include "Senales.h"
using namespace std;

void vistaConfiguracion( GtkWidget *widget, gpointer ptr ){
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

		// Establece su domicilio
		domicilio_empresa_registros_actualizar( "DomicilioEmpresaPropia", "ContenedorEmpresaPropiaDomicilios", stoi( (* resultado)[ "id_empresa" ] ) );
	}

	// Establece la señal de eliminar bascula
	conectar_senal( senal_domicilio_empresa_propia_agregar, G_CALLBACK( irHacia ), (void *)"AgregarDomicilio" );
	conectar_senal( senal_domicilio_empresa_propia_editar, G_CALLBACK( empresa_propia_domicilio_editar ), nullptr );
	conectar_senal( senal_domicilio_empresa_propia_eliminar, G_CALLBACK( domicilio_empresa_propia_eliminar ), nullptr );
	conectar_senal( senal_empresa_propia_editar, G_CALLBACK( empresa_propia_editar ), nullptr );

	conectar_senal( senal_domicilio_guardar_nuevo, G_CALLBACK( empresa_propia_domicilio_guardar_nuevo ), nullptr );
	conectar_senal( senal_domicilio_cancelar_nuevo, G_CALLBACK( regresarVista ), nullptr );
	conectar_senal( senal_bascula_registros_listar, G_CALLBACK( bascula_registros_listar ), nullptr );
	conectar_senal( botonConsultarUsuario, G_CALLBACK( usuario_ConsultarRegistros ), nullptr );
	//conectar_senal( botonSi, G_CALLBACK( basculaEliminar ), nullptr );

	// Se dirige a la vista
	irA( "Configuracion", false );
}

void vistaCrearRegistro( GtkWidget *widget, gpointer ptr )
{	
	// Folio
	stringstream folio;
	folio << setfill( '0' ) << setw( 7 ) << (folioActual + 1);
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ), folio.str().c_str() );
	
	// Fecha
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFechaInterno" ) ), tiempo_leer_fecha_corta().c_str() );
	
	// Limpia el formulario
	internoLimpiarFormulario();

	// Descuento
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ), "" );

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

void vistaInternoEditarRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data ){
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
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( buscar_objeto( "SiDescuentoInterno" ) ), TRUE );
		gtk_editable_set_editable( GTK_EDITABLE( buscar_objeto( "EntradaDescuentoInterno" ) ), TRUE );
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ), pesoString( ticket -> obtenerDescuento(), 2, false ).c_str() );
	}
	else{
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( buscar_objeto( "NoDescuentoInterno" ) ), TRUE );
		gtk_editable_set_editable( GTK_EDITABLE( buscar_objeto( "EntradaDescuentoInterno" ) ), FALSE );
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ), "" );
	}

	// Peso neto
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno" ) ), ( ticket -> estaPesoNetoEstablecido() ? pesoString( ticket -> obtenerPesoNeto(), 2 ) : "No establecido" ).c_str() );
		
	// Motivos y observaciones
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaObservacionesInterno" ) ), ticket -> obtenerObservaciones().c_str() );
	
	// Va hacia la vista
	irHacia( nullptr, (void *)"NuevoTicketInterno" );
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
	usuarioConsultado = buscarUsuarioPorNombreUsuario( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaConsultarUsuario" ) ) ), registrosUsuarios );
	if( usuarioConsultado == nullptr ){
		app_mostrar_mensaje( "El usuario que se desea consultar no fue encontrado." );
		return;
	}

	// Nombre
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "NombreConsultado" ) ), usuarioConsultado -> obtenerNombre().c_str() );

	// Apellidos
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "ApellidosConsultado" ) ), usuarioConsultado -> obtenerApellidos().c_str() );

	// Nombre de usuario
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "NombreUsuarioConsultado" ) ), usuarioConsultado -> obtenerNombreUsuario().c_str() );

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
