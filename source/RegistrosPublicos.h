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

void publicoRegistrarPendiente();
void publicoFinalizarPendiente();
void publicoRegistrarPesoBruto();
void publicoRegistrarPesoTara();
void publicoCalcularPesoNeto();
void publicoSeleccionarTipo();
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
