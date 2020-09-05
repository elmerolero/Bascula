#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <gtk/gtk.h>
#include "Ticket.h"

// Identificadores de las señales
extern guint entradaCampoId;
extern guint botonCampoId;
extern guint botonRegistrarViajeId;

// Ticket creado
extern Ticket *nuevo;

// Paso en el que va el programa
extern int pasoRegistroBasculaPublica;

void basculaPublicaNuevo( GtkWidget *widget, gpointer ptr );
void pasosRegistroBasculaPublica();
void leerPlacas();
void leerNombreConductor();
void leerNombreProducto();
void leerTipoViaje();
void convertirMayusculas( GtkEditable *editable, const gchar *text, gint length, gint *position, gpointer data );

#endif
