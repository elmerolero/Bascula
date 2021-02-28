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

void publicoRegistrarPendiente();
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

#endif
