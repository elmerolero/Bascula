#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <gtk/gtk.h>
#include <list>
#include "Ticket.h"

// Selectores
const int LECTOR_PESO_BRUTO = 1;
const int LECTOR_PESO_TARA = 2;

// Ticket a crear
extern Ticket *ticket;

void basculaInternaNuevo( GtkWidget *widget, gpointer ptr );
void registrarPesoBrutoInterno();
void registrarPesoTaraInterno();
void calcularPesoNeto();
void seleccionarTipoRegistro( GtkWidget *widget, gpointer ptr );
void habilitarDescuento( GtkWidget *widget, gpointer ptr );
void leerDescuento( GtkWidget *widget, gpointer ptr );
void registrarPendienteInterno();
void finalizarPendienteInterno();
void actualizarRegistrosInternos( std::list< Ticket * > &tickets );
void iniciarLectorPesoBruto();
void iniciarLectorPesoTara();
void cancelarRegistro( GtkWidget *widget, gpointer ptr );
void cancelarFinalizacion( GtkWidget *widget, gpointer ptr );
void convertirMayusculas( GtkEditable *editable, const gchar *text, gint length, gint *position, gpointer data );

#endif
