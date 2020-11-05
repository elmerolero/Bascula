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
void publicoActualizarRegistros( std::list< TicketPublico * > &ticketsPublicos );
 
#endif
