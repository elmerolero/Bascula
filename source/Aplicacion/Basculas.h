#ifndef BASCULAS_H
#define BASCULAS_H

#include "GestorRegistros.h"
#include <string>
#include <regex>
#include <gtk/gtk.h>

extern Signal senal_bascula_registros_listar;
extern Signal senal_bascula_seleccionar;
extern Signal senal_bascula_guardar;

extern Signal senal_bascula_registrar_pesaje;
extern Signal senal_bascula_cancelar_pesaje;

extern bool bascula_entrada_manual_habilitado;

// Vistas
void bascula_registros_listar( GtkWidget *widget, gpointer info );
void bascula_nuevo( GtkWidget *widget, gpointer info );
void bascula_editar( GtkWidget *widget, gpointer info );
void bascula_seleccionar( GtkListBox *box, GtkListBoxRow *row, gpointer info );
void bascula_lector_abrir( void );
void bascula_lector_seleccionar( GtkComboBox *lista, gpointer info );
bool bascula_lector_leer( gpointer info );
bool bascula_lector_iniciar( std::unordered_map< std::string, std::string > *bascula );
void bascula_lector_finalizar( void );
gboolean bascula_lector_cerrar( GtkWidget *widget, gpointer info );

// Actividades
void bascula_registros_actualizar( void );
void bascula_guardar_nuevo( GtkWidget *widget, gpointer info );
void bascula_eliminar( GtkWidget *widget, gpointer info );
void bascula_cancelar_nuevo( GtkWidget *widget, gpointer info );
void bascula_guardar_edicion( GtkWidget *widget, gpointer info );
void bascula_cancelar_edicion( GtkWidget *widget, gpointer info );
void bascula_registrar_pesaje( std::string etiquetaPeso, std::string etiquetaHora );

std::string bascula_nombre_validar( std::string nombre );
std::string bascula_puerto_validar( std::string puerto );
int bascula_tasa_baudios_validar( std::string puerto );
int bascula_largo_octeto_validar( std::string tamanio );
int bascula_bits_detencion_validar( std::string bits );
const char *bascula_paridad_leer_opcion( std::string paridad );
int bascula_paridad_validar( std::string paridad );

void bascula_perro_guardian( unsigned int milisegundos );
void basculaObtenerPuertosDisponibles();

#endif