#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <gtk/gtk.h>
#include <list>
#include <string>
#include "Ticket.h"
#include "sha256.h"

extern Registro *registro;

void crearRegistro( GtkWidget *widget, gpointer data );
void actualizarRegistro( GtkWidget *widget, gpointer data );
void eliminarRegistro( GtkWidget *widget, gpointer data );
void registrarEmpresa();
void cancelarAccion();
void registroCancelarEdicion( GtkWidget *widget, gpointer ptr );
void convertirMayusculas( GtkEditable *editable, const gchar *text, gint length, gint *position, gpointer data );
std::string pesoString( double, int );
std::string pesoString( double, int, bool );
std::string encriptar( std::string cadena, int numero );
std::string desencriptar( std::string cadena, int numero );
unsigned int obtenerFolioSelector( GtkListBoxRow *row );
std::string crearHash( std::string contrasena );

#endif
