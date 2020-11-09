#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <gtk/gtk.h>
#include <list>
#include <string>
#include "Ticket.h"

extern Registro *registro;

void nuevoEmpresa( GtkWidget *widget, gpointer ptr );
void nuevoProducto( GtkWidget *widget, gpointer ptr );
void actualizarEmpresa( GtkWidget *widget, gpointer ptr );
void actualizarProducto( GtkWidget *widget, gpointer ptr );
void eliminarProducto( GtkWidget * widget, gpointer ptr );
void eliminarEmpresa( GtkWidget *widget, gpointer ptr );
void alertaEliminarRegistro();
void cancelarOperacion();
void registroCancelarEdicion( GtkWidget *widget, gpointer ptr );
void convertirMayusculas( GtkEditable *editable, const gchar *text, gint length, gint *position, gpointer data );
std::string encriptar( std::string cadena, int numero );
std::string desencriptar( std::string cadena, int numero );

#endif
