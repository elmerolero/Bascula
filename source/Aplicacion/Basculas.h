#ifndef BASCULAS_H
#define BASCULAS_H

#include "GestorRegistros.h"
#include <string>
#include <regex>
#include <gtk/gtk.h>

extern Signal senal_bascula_guardar_nuevo;

void bascula_registros_actualizar( void );
void bascula_guardar_nuevo( GtkWidget *widget, gpointer info );
void bascula_cancelar_nuevo( GtkWidget *widget, gpointer info );
void bascula_guardar_edicion( GtkWidget *widget, gpointer info );
void bascula_cancelar_edicion( GtkWidget *widget, gpointer info );

std::string bascula_nombre_validar( std::string puerto );

#endif