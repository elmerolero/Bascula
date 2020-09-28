#include "LectorBascula.h"
#include <string>
#include "Widget.h"
using namespace std;

// Lector Bascula
LectorBascula lectorBascula;

void LectorBascula::abrir( Widget &interfaz )
{
	this -> interfaz = interfaz;
	
	// Muestra la ventana del lector
	interfaz.mostrarElemento( "VentanaLectorPeso" );
	
	// Establece las opciones de entrada
	actualizarOpciones();
	comboBoxLectorBasculaId = interfaz.conectarSenal( "BasculasDisponibles", "changed", G_CALLBACK( lectorBasculaIniciarOpcion ), nullptr );
	
	
	establecerActivo( true );
}

const char *LectorBascula::leer()
{
	// Establece el peso leído en la etiqueta
	return ( lecturaManualActivada() ? interfaz.obtenerTextoEntrada( "EntradaPeso" ) : interfaz.obtenerTextoEtiqueta( "EtiquetaPeso" ) );
}

void LectorBascula::actualizarOpciones()
{
	// Limpia las opciones que había anteriormente
	interfaz.limpiarComboBoxText( "BasculasDisponibles" );
	
	// Agrega las opciones seleccionar y manual
	interfaz.agregarOpcionComboBoxText( "BasculasDisponibles", "Seleccionar", "Seleccionar" );
	interfaz.establecerActivoComboBoxText( "BasculasDisponibles", "Seleccionar" );
	interfaz.agregarOpcionComboBoxText( "BasculasDisponibles", "Entrada manual", NULL );
}

void LectorBascula::iniciarOpcion()
{
	// Obtiene la opcion
	string opcion = interfaz.obtenerOpcionComboBoxText( "BasculasDisponibles" );
	interfaz.establecerTextoEntrada( "EntradaPeso", "" );
	interfaz.establecerTextoEtiqueta( "EtiquetaPeso", "0.0 Kg" );

	// Si la opcion seleccionada es "Introducción manual
	if( opcion.compare( "Entrada manual" ) == 0 ){
		establecerLecturaManual( true );
		interfaz.ocultarElemento( "EtiquetaPeso" );
		interfaz.mostrarElemento( "EntradaPeso" );
		interfaz.habilitarEdicionEntrada( "EntradaPeso" );
	}
	else if( opcion.compare( "Seleccionar" ) != 0 ){
		establecerLecturaManual( false );
		interfaz.deshabilitarEdicionEntrada( "EntradaPeso" );
		interfaz.ocultarElemento( "EntradaPeso" );
		interfaz.mostrarElemento( "EtiquetaPeso" );
	}
}

void LectorBascula::cerrar()
{
	// Oculta la ventana
	interfaz.ocultarElemento( "VentanaLectorPeso" );
	
	// Deshabilita la entrada peso
	interfaz.deshabilitarEdicionEntrada( "EntradaPeso" );
	
	// Limpia lo que había en la entrada
	interfaz.establecerTextoEntrada( "EntradaPeso", "" );
	
	// Desconecta la señal
	interfaz.desconectarSenal( "BotonRegistrarPeso", botonLectorBasculaId );
	interfaz.desconectarSenal( "BasculasDisponibles", comboBoxLectorBasculaId );
	
	establecerActivo( false );
}

void LectorBascula::establecerIdSenal( guint senal )
{
	botonLectorBasculaId = senal;
}

void LectorBascula::establecerLecturaManual( bool lecturaManual )
{
	this -> lecturaManual = lecturaManual;
}

bool LectorBascula::lecturaManualActivada() const
{
	return lecturaManual;
}

void LectorBascula::establecerActivo( bool activo )
{
	this -> activado = activo;
}

bool LectorBascula::estaActivo() const
{
	return activado;
}

// Establece las opciones de la báscula
void lectorBasculaIniciarOpcion( GtkWidget *widget, gpointer ptr )
{
	lectorBascula.iniciarOpcion();
}

// Cierra la bascula
void lectorBasculaCerrar( GtkWidget *widget, gpointer ptr )
{
	lectorBascula.cerrar();
}
