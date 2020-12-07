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
void internoActualizarRegistros( std::list< Ticket * > &tickets, std::string idContenedor );
void internoObtenerPorFecha( std::list <Ticket *> &registros, std::string fecha );
void internoCancelarRegistro();
void internoCancelarFinalizacion();
void internoImprimirSeleccionado();
void internoAlertaEliminar();
void internoEliminarSeleccionado();
void internoSeleccionarDia();
void internoObtenerRegistrosRango();
void internoGenerarInforme();

#endif
