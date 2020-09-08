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
	// Establece el folio para la primera vista del ticket
	stringstream folio;
	folio << "Folio: " << setfill( '0' ) << setw( 7 ) << (folioActual + 1);
	interfaz.establecerTextoEtiqueta( "EntradaFolioInterno", folio.str() );
	
	// Establece la fecha del ticket
	interfaz.establecerTextoEtiqueta( "EntradaFechaInterno", "Fecha: " + obtenerFecha() );
	
	// C
	
	irHacia( nullptr, (void *)"NuevoTicketInterno" );
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
