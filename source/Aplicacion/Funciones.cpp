#include "Funciones.h"
#include "Aplicacion.h"
#include "Registro.h"
#include "Funciones.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "GestorRegistros.h"
#include "Vistas.h"
#include "Producto.h"
using namespace std;

// Banderas
bool descuentoCalculado = false;

// Registro a crear
Registro *registro;
 
void crearRegistro( GtkWidget *widget, gpointer data ){
	cout << "crear_registro" << endl;

	try{
		// Lee el nombre del nuevo registro
		string nombreRegistro = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreNuevoRegistro" ) ) );
		producto_validar_nombre( nombreRegistro );

		// Verifica que no exista un registro con ese nombre
		stringstream consulta;
		consulta << "select * from " << (char *)data << " where nombre like '%" << nombreRegistro << "'";
		database.open( databaseFile );
		database.query( consulta.str() );
		database.close();

		if( results.size() > 0 ){
			cout << "llegue a qui" << endl;
			app_mostrar_error( "El registro que desea agregar ya existe." );
			return;
		}

		consulta.str( "" );
		consulta << "insert into " << (char *)data << " values( null, '" << nombreRegistro << "', null, null )";
	   	database.open( databaseFile );
		database.query( consulta.str() );
	   	database.close();

		// Actualizar la lista de registros
		//contenedor -> actualizarListaRegistros();
		
		// Regresa hacia atrás
		regresarVista();
		regresarVista();

		// Muestra que el registro se creo correctamente y vuelve hacia la vista de registros
		app_mostrar_error( "Registro creado correctamente." );

		producto_listar_registros( nullptr, nullptr );
	}
	catch( invalid_argument &ia ){
		app_mostrar_error( ia.what() );
	}
}

void actualizarRegistro( GtkWidget *widget, gpointer data )
{
	try{
		// Convierte el apuntador tipo void en apuntador tipo ContenedorRegistros
		ContenedorRegistros *contenedor = static_cast< ContenedorRegistros * >( data );

		// Obtenemos el nuevo nombre que se desea establecer
		string nuevoNombre = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EtiquetaClaveEditarRegistro" ) ) );

		// Obtiene el registro que se desea actualizar a través de su clave
		Registro *registro = contenedor -> buscarRegistroPorClave( stoi( nuevoNombre ) );
		if( registro == nullptr ){
			throw runtime_error( "Ha ocurrido un error al intentar recuperar el registro que se deseaba editar." );
		}

		// Verifica que no exista un registro con el nuevo nombre que desea establecer
		Registro *coincidencia = contenedor -> buscarRegistroPorNombre( nuevoNombre );
    	if( coincidencia != nullptr ){
	    	app_mostrar_error( "Ya existe un registro con ese nombre." );
			return;
    	}

		// Establece el nuevo nombre del registro
		registro -> establecerNombre( gtk_entry_get_text( GTK_ENTRY( "EntradaNombreEditarRegistro" ) ) );

		// Actualiza el nombre en la base de datos
		contenedor -> actualizarRegistro( registro );

		// Actualizar la lista de registros
		contenedor -> actualizarListaRegistros();
		
		// Regresar
		regresarVista();
		regresarVista();

		producto_listar_registros( nullptr, nullptr );

		// Muestra un mensaje de registro exitoso
		app_mostrar_error( "Registro actualizado correctamente." );
	}
	catch( invalid_argument &ia ){
		app_mostrar_error( ia.what() );
	}
}

void eliminarRegistro( GtkWidget *widget, gpointer data ){
	// Obtiene el id del registro
	//unsigned int id = stoi(  );

	// Comando de consulta
	stringstream consulta;

	// 
	/*consulta << "delete * from where id_producto = " 
			 << gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistros" ) ) ) ) ) ) )
			 << */
	consulta << "delete from " << (char *)data << " where id_producto = " 
			 << gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistros" ) ) ) ) ) ) );
			
	database.open( databaseFile );
	database.query( consulta.str() );
	database.close();

	// Actualiza la vista de registros
	//contenedor -> actualizarListaRegistros();

	// Cierra el mensaje de alerta
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );

	regresarVista();
	producto_listar_registros( nullptr, nullptr );
}

void cancelarAccion() 
{
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );
}

// 
void registroCancelarEdicion( GtkWidget *widget, gpointer ptr ){
	// Oculta el formato de entrada de datos y muestra la etiqueta con el nombre orignal
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "EntradaNombreRegistro" ) ) );
	gtk_widget_show( GTK_WIDGET( buscar_objeto( "EtiquetaNombreRegistro" ) ) );
	
	// Oculta los botones de control de edicion
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "ControlesEdicionRegistro" ) ) );
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

// Registra el nombre de la empresa
void registrarEmpresa()
{
	string nombreEmpresa = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreEmpresaPropia" ) ) );

	try{
		//registrarNombreEmpresa( nombreEmpresa );
		//cargarNombreEmpresa();
		irHacia( nullptr, (void *)"Inicio" );
	}
	catch( invalid_argument &ia ){
		app_mostrar_error( ia.what() );
	}
}

// Peso a string
string pesoString( double peso, int precision )
{
	stringstream pesoStr;
	pesoStr << fixed << setprecision( precision ) << peso << " Kg" << endl;
	return pesoStr.str();
}

// Peso a string
string pesoString( double peso, int precision, bool mostrarUnidad )
{
	stringstream pesoStr;
	pesoStr << fixed << setprecision( precision ) << peso << ( mostrarUnidad ? " Kg" : "" );
	return pesoStr.str();
}

// Encripta la cadena dado un número
string encriptar( string texto, int numero )
{
	// Entrada
	string str = texto;

	// Proceso
	for( char& c : str ){
		c = c + numero;
	}

	return str;
}

// Desencripta la cadena dado un número
string desencriptar( string texto, int numero )
{
	// Entrada
	string str = texto;

	// Proceso
	for( char& c : str ){
		c = c - numero;
	}

	return str;
}

unsigned int obtenerFolioSelector( GtkListBoxRow *row )
{
	GtkWidget *item = gtk_bin_get_child( GTK_BIN( row ) );
	return stoi( gtk_widget_get_name( item ) );
}

string crearHash( std::string contrasena )
{
	SHA256 hashTool;
	return hashTool( contrasena );
}