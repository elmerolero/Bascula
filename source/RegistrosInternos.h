/***********************************************************
 * Registros Internos
 * Contiene todas las funciones relacionadas a esto que son
 * llamadas cuando se lanza un evento.
 *
 ***********************************************************/
#ifndef REGISTROS_INTERNOS_H
#define REGISTROS_INTERNOS_H
#include <list>
#include "Ticket.h"

// Ticket a crear
extern Ticket *ticket;

void internoRegistrarPendiente();
void internoFinalizarPendiente();
void internoRegistrarPesoBruto();
void internoRegistrarPesoTara();
void internoCalcularPesoNeto();
void internoHabilitarDescuento();
void internoLeerDescuento();
void internoSeleccionarTipo();
void internoActualizarRegistros( std::list< Ticket * > &tickets );
void internoCancelarRegistro();
void internoCancelarFinalizacion();

#endif
