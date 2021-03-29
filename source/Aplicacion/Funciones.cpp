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
using namespace std;

// Banderas
bool descuentoCalculado = false;

// Registro a crear
Registro *registro;

void crearRegistro( GtkWidget *widget, gpointer data )
{
	// Convierte el apuntador void a un apuntador ContenedorRegistros
	ContenedorRegistros *contenedor = static_cast< ContenedorRegistros * >( data );

	// Lee el nombre del nuevo registro
	string nombreRegistro = interfaz.obtenerTextoEntrada( "EntradaNombreNuevoRegistro" );
	
	// Verifica que no exista un registro con ese nombre
	Registro *registro = contenedor -> buscarRegistroPorNombre( nombreRegistro );
	if( registro != nullptr ){
		mostrarMensajeError( "El registro que desea agregar ya existe." );
		return;
	}

	try{
		// Añade el nuevo registro y e valida que hubiera sido creada satisfactoriamente
		registro = contenedor -> agregarNuevoRegistro( nombreRegistro );
		if( registro == nullptr ){
			mostrarMensajeError( "Ha ocurrido un error al crear un nuevo registro en " + contenedor -> obtenerNombrePlural() );
			return;
		}

		// Actualizar la lista de registros
		contenedor -> actualizarListaRegistros();
		
		// Regresa hacia atrás
		regresarVista();

		// Muestra que el registro se creo correctamente y vuelve hacia la vista de registros
		mostrarMensajeError( "Registro creado correctamente." );
	}
	catch( invalid_argument &ia ){
		mostrarMensajeError( ia.what() );
	}
}

void actualizarRegistro( GtkWidget *widget, gpointer data )
{
	try{
		// Convierte el apuntador tipo void en apuntador tipo ContenedorRegistros
		ContenedorRegistros *contenedor = static_cast< ContenedorRegistros * >( data );

		// Obtenemos el nuevo nombre que se desea establecer
		string nuevoNombre = interfaz.obtenerTextoEtiqueta( "EtiquetaClaveEditarRegistro" );

		// Obtiene el registro que se desea actualizar a través de su clave
		Registro *registro = contenedor -> buscarRegistroPorClave( stoi( nuevoNombre ) );
		if( registro == nullptr ){
			throw runtime_error( "Ha ocurrido un error al intentar recuperar el registro que se deseaba editar." );
		}

		// Verifica que no exista un registro con el nuevo nombre que desea establecer
		Registro *coincidencia = contenedor -> buscarRegistroPorNombre( nuevoNombre );
    	if( coincidencia != nullptr ){
	    	mostrarMensajeError( "Ya existe un registro con ese nombre." );
			return;
    	}

		// Establece el nuevo nombre del registro
		registro -> establecerNombre( interfaz.obtenerTextoEntrada( "EntradaNombreEditarRegistro" ) );

		// Actualiza el nombre en la base de datos
		contenedor -> actualizarRegistro( registro );

		// Actualizar la lista de registros
		contenedor -> actualizarListaRegistros();
		
		// Regresar
		regresarVista();

		// Muestra un mensaje de registro exitoso
		mostrarMensajeError( "Registro actualizado correctamente." );
	}
	catch( invalid_argument &ia ){
		mostrarMensajeError( ia.what() );
	}
}

void eliminarRegistro( GtkWidget *widget, gpointer data )
{
	// Obtiene el contenedor del que se desea eliminar el registro
	ContenedorRegistros *contenedor = static_cast< ContenedorRegistros * >( data );

	// Obtiene la clave del registro seleccionado
	unsigned int clave = stoi( interfaz.obtenerWidgetSeleccionadoListBox( "ContenedorRegistros" ) );

	// Recupera el registro por si clave
	Registro *registro = contenedor -> buscarRegistroPorClave( clave );
	if( registro == nullptr ){
		throw runtime_error( "Ocurrió un problema al recuperar el registro seleccionado." );
	}

	// Elimina el registro obtenido
	contenedor -> eliminarRegistro( registro );

	// Actualiza la vista de registros
	contenedor -> actualizarListaRegistros();

	// Cierra el mensaje de alerta
	interfaz.ocultarElemento( "VentanaSiNo" );
}

void cancelarAccion() 
{
	interfaz.ocultarElemento( "VentanaSiNo" );
}

// 
void registroCancelarEdicion( GtkWidget *widget, gpointer ptr )
{
	// Oculta el formato de entrada de datos y muestra la etiqueta con el nombre orignal
	interfaz.ocultarElemento( "EntradaNombreRegistro" );
	interfaz.mostrarElemento( "EtiquetaNombreRegistro" );
	
	// Oculta los botones de control de edicion
	interfaz.ocultarElemento( "ControlesEdicionRegistro" );
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
	string nombreEmpresa = interfaz.obtenerTextoEntrada( "EntradaNombreEmpresaPropia" );

	try{
		registrarNombreEmpresa( nombreEmpresa );
		cargarNombreEmpresa();
		irHacia( nullptr, (void *)"Inicio" );
	}
	catch( invalid_argument &ia ){
		mostrarMensajeError( ia.what() );
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
