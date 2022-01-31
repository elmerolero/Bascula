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

extern TicketPublico *registroPublico;
extern bool publicoPendiente;

// Señales
extern Signal senal_listar_registros_pendientes;

extern Signal senal_publico_nuevo_pendiente;
extern Signal senal_publico_editar_pendiente;


void publico_registros_listar_pendientes( GtkWidget *widget, gpointer info );
void publico_listar_registros_rango( GtkWidget *widget, gpointer info );
void publico_nuevo_pendiente( GtkWidget *widget, gpointer info );
void publico_pendiente_guardar_nuevo( GtkWidget *widget, gpointer info );

void publico_registros_actualizar_pendientes( void );
void publico_registros_actualizar_rango( void );

void publicoFinalizarPendiente();
void publicoRegistrarPesoBruto();
void publicoRegistrarPesoTara();
double publicoCalcularPesoNeto( double pesoBruto, double pesoTara );
void publicoActualizarPesoNeto();
void publicoCancelarRegistro();
void publicoCancelarFinalizacion();
void publicoObtenerPorFecha( std::list <TicketPublico *> &registros, std::string fecha );
void publicoActualizarRegistros( std::list< TicketPublico * > &ticketsPublicos, std::string idContenedor );
void publicoSeleccionarDia();
void publicoAlertaEliminar();
void publicoEliminarSeleccionado();
void publicoImprimirSeleccionado(); 
void publicoGenerarInforme();
void publicoObtenerRegistrosRango();

// Validación de datos
std::string publico_conductor_validar( std::string nombre );
std::string publico_placas_validar( std::string placas );

#endif
