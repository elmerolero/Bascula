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
	pesoStr << fixed << setprecision( precision ) << peso << " Kg";
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