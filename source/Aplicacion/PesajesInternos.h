/***********************************************************
 * Registros Internos
 * Contiene todas las funciones relacionadas a esto que son
 * llamadas cuando se lanza un evento.
 *
 ***********************************************************/
#ifndef REGISTROS_INTERNOS_H
#define REGISTROS_INTERNOS_H

#include <list>
#include <gtk/gtk.h>
#include "GestorRegistros.h"
#include "PesajesPublicos.h"

const int TIPO_REGISTRO_ENTRADA = 0;
const int TIPO_REGISTRO_SALIDA = 1;

extern GtkEntryCompletion *completadorInterno;
extern GtkEntryCompletion *completadorInternoProducto;
extern GtkEntryCompletion *completadorInternoConductor;
extern GtkEntryCompletion *completadorInternoPlacas;

extern Signal senal_interno_opcion;
extern Signal senal_interno_numero_placas;

// Señales
extern Signal senal_interno_pendientes_listar;

extern Signal senal_interno_editar_pendiente;
extern Signal senal_interno_pendiente_finalizar;

extern Signal senal_interno_leer_bruto;
extern Signal senal_interno_leer_tara;
extern Signal senal_interno_calcular_descuento;

void interno_iniciar( void );

void interno_registros_listar_pendientes( GtkWidget *widget, gpointer info );
void interno_registros_listar( GtkWidget *widget, gpointer info );
void interno_registros_listar_rango( GtkWidget *widget, gpointer info );
void interno_pendiente_nuevo( GtkWidget *widget, gpointer info );
void interno_pendiente_seleccionar( GtkWidget *widget, GtkListBoxRow *row, gpointer data );
void interno_pendiente_guardar_nuevo( GtkWidget *widget, gpointer info );
void interno_pendiente_guardar_edicion( GtkWidget *widget, gpointer info );
void interno_pendiente_finalizar( GtkWidget *widget, gpointer info );

void interno_leer_bruto( GtkWidget *widget, gpointer info );
void interno_registrar_bruto( GtkWidget *widget, gpointer info );
void interno_leer_tara( GtkWidget *widget, gpointer info );
void interno_registrar_tara( GtkWidget *widget, gpointer info );

void interno_calcular_neto( void );
void interno_registros_exportar( GtkWidget *widget, gpointer info );

void interno_registros_actualizar_pendientes( void );

void interno_seleccionar( GtkWidget *widget, GtkListBoxRow *row, gpointer data );
static void interno_imprimir(GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkWidget *data );
void interno_eliminar( GtkWidget *widget, gpointer data );

static void interno_hacer_impresion( GtkWidget* widget, GtkWidget* info );
static void interno_imprimir(GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkWidget *data);

// Validación de datos
std::string interno_conductor_validar( std::string nombre );
std::string interno_placas_validar( std::string placas );

#endif
