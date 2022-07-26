/***********************************************************
 * Registros Públicos
 * Contiene todas las funciones relacionadas a esto que son
 * llamadas cuando se lanza un evento.
 *
 ***********************************************************/
#ifndef REGISTROS_PUBLICOS_H
#define REGISTROS_PUBLICOS_H

#include <list>
#include "GestorRegistros.h"
#include "TicketPublico.h"

extern GtkEntryCompletion *completadorPublicoProducto;

extern Signal senal_publico_opcion;
extern Signal senal_publico_numero_placas;

// Señales
extern Signal senal_publico_pendientes_listar;

extern Signal senal_publico_editar;
extern Signal senal_publico_editar_pendiente;
extern Signal senal_publico_pendiente_finalizar;

extern Signal senal_publico_leer_bruto;
extern Signal senal_publico_leer_tara;

extern Signal senal_pesajes_nuevo_pendiente;
extern Signal senal_pesajes_seleccionar;
extern Signal senal_pesajes_periodo_seleccionar;
extern Signal senal_pesajes_exportar_seleccionados;
extern Signal senal_pesajes_obtener_seleccionados;


void publico_iniciar();
void publico_registros_listar_pendientes( GtkWidget *widget, gpointer info );
void publico_registros_listar( GtkWidget *widget, gpointer info );
void publico_registros_listar_rango( GtkWidget *widget, gpointer info );
void publico_pendiente_nuevo( GtkWidget *widget, gpointer info );
void publico_pendiente_seleccionar( GtkWidget *widget, GtkListBoxRow *row, gpointer data );
void publico_pendiente_guardar_nuevo( GtkWidget *widget, gpointer info );
void publico_pendiente_guardar_edicion( GtkWidget *widget, gpointer info );
void publico_pendiente_finalizar( GtkWidget *widget, gpointer info );

void publico_leer_bruto( GtkWidget *widget, gpointer info );
void publico_registrar_bruto( GtkWidget *widget, gpointer info );
void publico_leer_tara( GtkWidget *widget, gpointer info );
void publico_registrar_tara( GtkWidget *widget, gpointer info );

void publico_calcular_neto( void );
void publico_registros_exportar( GtkWidget *widget, gpointer info );

void publico_registros_actualizar_pendientes( void );

void publico_seleccionar( GtkWidget *widget, GtkListBoxRow *row, gpointer data );
static void publico_imprimir(GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkWidget *data );
void publico_eliminar( GtkWidget *widget, gpointer data );

static void publico_hacer_impresion_parcial(void);
static void  publico_imprimir_parcial(GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkWidget *data);
static void publico_hacer_impresion( GtkWidget* widget, GtkWidget* info );
static void publico_imprimir(GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkWidget *data);

// Validación de datos
std::string publico_conductor_validar( std::string nombre );
std::string publico_placas_validar( std::string placas );

#endif
