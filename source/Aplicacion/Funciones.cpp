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

void nuevoEmpresa( GtkWidget *widget, gpointer ptr )
{
	// Lee el nombre de la nueva empresa
	string empresaNueva = interfaz.obtenerTextoEntrada( "EntradaNombreNuevoRegistro" );
	
	// Verifica que no exista un registro con ese nombre
	Registro *empresa = empresas.buscarRegistroPorNombre( empresaNueva );
	if( empresa != nullptr ){
		interfaz.establecerTextoEtiqueta( "MensajeConsultarRegistroError", "El registro que desea agregar ya existe." );
		interfaz.mostrarElemento( "MensajeConsultarRegistroError" );
		return;
	}
	
	try{
		// Añade la nueva empresa
		empresa = empresas.agregarNuevoRegistro( empresaNueva );
		
		// Verifica que se hubiera creado
		if( empresa == nullptr ){
			interfaz.establecerTextoEtiqueta( "MensajeConsultarRegistroError", "Ha ocurrido un error." );
			interfaz.mostrarElemento( "MensajeConsultarRegistroError" );
			return;
		}
		
		// Muestra que el registro se creo correctamente y vuelve hacia la vista de registros de empresas
		mostrarMensaje( "Empresa creada correctamente." );
		vistaRegistrosEmpresas( nullptr, nullptr );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeConsultarRegistroError", ia.what() );
		interfaz.mostrarElemento( "MensajeConsultarRegistroError" );
	}
}

void nuevoProducto( GtkWidget *widget, gpointer ptr )
{
	// Lee el nombre de la nueva empresa
	string productoNuevo = interfaz.obtenerTextoEntrada( "EntradaNombreNuevoRegistro" );
	
	// Verifica que no exista un registro con ese nombre
	Registro *producto = productos.buscarRegistroPorNombre( productoNuevo );
	if( producto != nullptr ){
		interfaz.establecerTextoEtiqueta( "MensajeConsultarRegistroError", "El registro que desea agregar ya existe." );
		interfaz.mostrarElemento( "MensajeConsultarRegistroError" );
		return;
	}
	
	try{
		// Añade la nueva empresa
		producto = productos.agregarNuevoRegistro( productoNuevo );
		
		// Verifica que se hubiera creado
		if( producto == nullptr ){
			interfaz.establecerTextoEtiqueta( "MensajeConsultarRegistroError", "Ha ocurrido un error." );
			interfaz.mostrarElemento( "MensajeConsultarRegistroError" );
			return;
		}
		
		// Muestra que el registro se creo correctamente y vuelve hacia la vista de registros de producto
		mostrarMensaje( "Producto creado correctamente." );
		vistaRegistrosProductos( nullptr, nullptr );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeConsultarRegistroError", ia.what() );
		interfaz.mostrarElemento( "MensajeConsultarRegistroError" );
	}
}

void actualizarEmpresa( GtkWidget *widget, gpointer ptr )
{
	try{
		empresas.actualizarRegistro( registro );
		
		// Actualiza los datos de la etiqueta
		interfaz.establecerTextoEtiqueta( "EtiquetaNombreRegistro", registro -> obtenerNombre() );
		
		// Oculta los controles de edicion
		registroCancelarEdicion( nullptr, nullptr );
		
		// Muestra mensaje de registro exitoso
		interfaz.ocultarElemento( "MensajeConsultarRegistroError" );
		mostrarMensaje( "Registro editado correctamente." );
	}
	catch( invalid_argument ia ){
		interfaz.establecerTextoEtiqueta( "MensajeConsultarRegistroError", ia.what() );
		interfaz.mostrarElemento( "MensajeConsultarRegistroError" );
	}
}

void actualizarProducto( GtkWidget *widget, gpointer ptr )
{
	try{
		productos.actualizarRegistro( registro );
		
		// Actualiza los datos de la etiqueta
		interfaz.establecerTextoEtiqueta( "EtiquetaNombreRegistro", registro -> obtenerNombre() );
		
		// Oculta los controles de edicion
		registroCancelarEdicion( nullptr, nullptr );
		
		// Muestra mensaje de registro exitoso
		interfaz.ocultarElemento( "MensajeConsultarRegistroError" );
		mostrarMensaje( "Registro editado correctamente." );
	}
	catch( invalid_argument ia ){
		interfaz.establecerTextoEtiqueta( "MensajeConsultarRegistroError", ia.what() );
		interfaz.mostrarElemento( "MensajeConsultarRegistroError" );
	}
}

void alertaEliminarRegistro()
{
	interfaz.establecerTextoEtiqueta( "MensajeAlerta", "ALERTA: Si el existen registros de pesaje que hagan\n"
													   "referencia a este registro, también serán eliminados.\n"
													   "¿Estás seguro que deseas eliminarlo?" );
	interfaz.mostrarElemento( "VentanaSiNo" );
}

void eliminarEmpresa( GtkWidget *widget, gpointer ptr )
{
	interfaz.ocultarElemento( "VentanaSiNo" );
	empresas.eliminarRegistro( registro );
	vistaRegistrosEmpresas( nullptr, nullptr );
}

void eliminarProducto( GtkWidget *widget, gpointer ptr )
{
	interfaz.ocultarElemento( "VentanaSiNo" );
	productos.eliminarRegistro( registro );
	vistaRegistrosProductos( nullptr, nullptr );
}

void cancelarOperacion() 
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
		interfaz.establecerTextoEtiqueta( "MensajeErrorRegistrarNombreEmpresa", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorRegistrarNombreEmpresa" );
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
