#ifndef BASCULAS_H
#define BASCULAS_H

#include "GestorRegistros.h"
#include <string>
#include <regex>
#include <gtk/gtk.h>

extern Signal senal_bascula_registros_listar;
extern Signal senal_bascula_seleccionar;
extern Signal senal_bascula_guardar_nuevo;

// Vistas
void bascula_registros_listar( GtkWidget *widget, gpointer info );
void bascula_seleccionar( GtkListBox *box, GtkListBoxRow *row, gpointer info );

// Actividades
void bascula_registros_actualizar( void );
void bascula_guardar_nuevo( GtkWidget *widget, gpointer info );
void bascula_cancelar_nuevo( GtkWidget *widget, gpointer info );
void bascula_guardar_edicion( GtkWidget *widget, gpointer info );
void bascula_cancelar_edicion( GtkWidget *widget, gpointer info );

std::string bascula_nombre_validar( std::string nombre );
std::string bascula_puerto_validar( std::string puerto );
int bascula_tasa_baudios_validar( std::string puerto );
int bascula_largo_octeto_validar( std::string tamanio );
int bascula_bits_detencion_validar( std::string bits );
int bascula_paridad_validar( std::string paridad );

#endif