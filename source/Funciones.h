#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <gtk/gtk.h>
#include "Ticket.h"

// Selectores
const int LECTOR_PESO_BRUTO = 1;
const int LECTOR_PESO_TARA = 2;

// Identificadores de las señales
extern guint botonLectorBasculaId;
extern guint comboBoxLectorBasculaId;

// Ticket a crear
extern Ticket *ticket;

void basculaInternaNuevo( GtkWidget *widget, gpointer ptr );
void abrirLectorBascula( GtkWidget *widget, gpointer ptr );
const char *leerBascula();
void cerrarLectorBascula();
void establecerOpcionesBascula();
void iniciarOpcionBascula();
void registrarPesoBrutoInterno();
void registrarPesoTaraInterno();
void calcularPesoNeto();
void seleccionarTipoRegistro( GtkWidget *widget, gpointer ptr );
void habilitarDescuento( GtkWidget *widget, gpointer ptr );
void registrarTicket();
void aceptar( GtkWidget *widget, gpointer ptr );
void actualizarTicketsPendientes();
void completarTicket( Ticket *ticket );
void convertirMayusculas( GtkEditable *editable, const gchar *text, gint length, gint *position, gpointer data );

#endif
