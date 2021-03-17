#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <gtk/gtk.h>
#include <list>
#include <string>
#include "Ticket.h"

extern Registro *registro;

void crearRegistro( GtkWidget *widget, gpointer data );
void actualizarRegistro( GtkWidget *widget, gpointer data );

void eliminarProducto( GtkWidget * widget, gpointer ptr );
void eliminarEmpresa( GtkWidget *widget, gpointer ptr );
void alertaEliminarRegistro();
void registrarEmpresa();
void cancelarOperacion();
void registroCancelarEdicion( GtkWidget *widget, gpointer ptr );
void convertirMayusculas( GtkEditable *editable, const gchar *text, gint length, gint *position, gpointer data );
std::string pesoString( double, int );
std::string pesoString( double, int, bool );
std::string encriptar( std::string cadena, int numero );
std::string desencriptar( std::string cadena, int numero );
unsigned int obtenerFolioSelector( GtkListBoxRow *row );

#endif
