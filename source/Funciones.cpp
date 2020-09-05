#include "Funciones.h"
#include "Aplicacion.h"
#include "Registro.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
using namespace std;

guint entradaCampoId;
guint botonCampoId;
guint botonRegistrarViajeId;

Ticket *nuevo = nullptr;

int pasoEfectuado = 1;

// Crea un nuevo ticket
void basculaPublicaNuevo( GtkWidget *widget, gpointer ptr )
{
	// Crea un nuevo ticket
	nuevo = new Ticket();
	if( nuevo == nullptr ){
		throw runtime_error( "No hay memoria suficiente para crear un nuevo registro de peso." ); 
	}
	
	// Establecemos su folio
	nuevo -> establecerFolio( ++folioActual );
	
	// Establecemos una empresa no definida
	Registro empresa;
	empresa.establecerClave( 0 );
	empresa.establecerNombre( "NO ADMITE" );
	
	// Establece el folio para la primera vista del ticket
	stringstream folio;
	folio << "Folio: " << setfill( '0' ) << setw( 7 ) << nuevo -> obtenerFolio();
	interfaz.establecerTextoEtiqueta( "EntradaFolio", folio.str() );
	
	// Establece la señal para ir a la siguiente parte del registro
	botonCampoId = interfaz.conectarSenal( "BotonCampoSiguiente", "clicked", G_CALLBACK( pasosRegistroBasculaPublica ), nullptr );
	entradaCampoId = interfaz.conectarSenal( "EntradaCampo", "activate", G_CALLBACK( pasosRegistroBasculaPublica ), nullptr );
	botonRegistrarViajeId = interfaz.conectarSenal( "BotonRegistrarViaje", "activate", G_CALLBACK( pasosRegistroBasculaPublica ), nullptr );
	
	leerPlacas();
}

void pasosRegistroBasculaPublica()
{	
	// Registro
	Registro *producto = nullptr;
		
	try{
		// Obtiene el texto introducido
		string textoEntrada = interfaz.obtenerTextoEntrada( "EntradaCampo" );
		string opcionSeleccionada = ;
		
		// Valida y establece los datos dados de acuerdo en el paso que se encuentre
		switch( pasoEfectuado ){
			case 1:
				nuevo -> establecerNumeroPlacas( textoEntrada );
				leerNombreConductor();
				pasoEfectuado++;
				break;
			case 2:
				nuevo -> establecerNombreConductor( textoEntrada );
				leerNombreProducto();
				pasoEfectuado++;
				break;
			case 3:
				producto = buscarRegistro( textoEntrada, productos );
				if( producto == nullptr ){
					producto = new Registro();
					producto -> establecerClave( ++claveProductoActual );
					producto -> establecerNombre( textoEntrada );
					productos.push_back( producto );
				}
				nuevo -> establecerProducto( producto );
				leerTipoViaje();
				pasoEfectuado++;
				break;
			case 4:
				
				break;
		}
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCampo", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCampo" );
		if( producto != nullptr ){
			delete producto;
		}
	}
}

// Vista para leer el número de placa
void leerPlacas()
{
	// Establece la vista para la introducción de texto
	irHacia( nullptr, (void *)"IntroducirTexto" );
	
	// Establece el nombre del campo a introducir
	interfaz.establecerTextoEtiqueta( "TituloCampo", "Placas" );
	
	// Establece el campo a 7 caracteres
	interfaz.establecerTamanoEntrada( "EntradaCampo", 7 );
}

// Registra el número de placas y procede a solicitar el nombre del conductor
void leerNombreConductor()
{
	// Oculta cualquier error mostrado anteriormente
	interfaz.ocultarElemento( "MensajeErrorCampo" );
		
	// Establece el nombre del campo a introducir
	interfaz.establecerTextoEtiqueta( "TituloCampo", "Nombre del conductor" );
		
	// Limpia el campo de entrada de datos
	interfaz.establecerTextoEntrada( "EntradaCampo", "" );
	
	// Establece el campo a 40 caracteres
	interfaz.establecerTamanoEntrada( "EntradaCampo", 35 );
}

// Registra el nombre del conductor y procede a solicitar el producto que lleva
void leerNombreProducto()
{
	// Oculta cualquier error mostrado anteriormente
	interfaz.ocultarElemento( "MensajeErrorCampo" );
	
	// Establece el nombre del campo a introducir
	interfaz.establecerTextoEtiqueta( "TituloCampo", "Nombre del producto" );
	
	// Limpia el campo de entrada de datos
	interfaz.establecerTextoEntrada( "EntradaCampo", "" );
	
	// Establece el modelo adecuado
	gtk_entry_completion_set_model( completador, GTK_TREE_MODEL( listaProductos ) );
}

// Indica el tipo de viaje que se desea realizar
void leerTipoViaje()
{
	// Establece la vista
	irHacia( nullptr, (void *)"SeleccionarOpcion" );
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
