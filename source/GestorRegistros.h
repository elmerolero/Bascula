#ifndef GESTOR_REGISTROS_H
#define GESTOR_REGISTROS_H
#include <list>
#include "Database.h"
#include "Usuario.h"
#include "Ticket.h"
#include "TicketPublico.h"
#include "ContenedorRegistros.h"

// Base de datos
extern Database database;

// Usuario
extern Usuario usuario;

// Contador de folio de registros internos y públicos
extern unsigned int folioActual;
extern unsigned int folioActualPublico;

// Lista de registros internos
extern std::list< Ticket * > registrosInternosPendientes;
extern std::list< Ticket * > registrosConsultados;

// Lista de registros publicos
extern std::list< TicketPublico * > registrosPublicosPendientes;
extern std::list< TicketPublico * > registrosPublicosConsultados;

// Registros de los productos y empresas
extern ContenedorRegistros productos;
extern ContenedorRegistros empresas;

// Obtiene el contador de los folios
void obtenerFolioActual();
void obtenerFolioActualPublico();

// Obtiene los tickets registrados del día
void obtenerRegistrosPublicosPendientes();
void obtenerRegistrosInternosPendientes();

// Realiza un registro del ticket
void crearRegistroPendiente( Ticket *ticket );

// finaliza un registro
void finalizarRegistro( Ticket *ticket );
void finalizarRegistroPublico( TicketPublico *registroPublico );

// Crea un registro publico pendiente
void crearRegistroPublicoPendiente( TicketPublico *registroPublico );

// Busca un ticket por folio
Ticket *buscarRegistroInternoPorFolio( unsigned int folio, std::list< Ticket * > registros );

// Busca un ticket por numero de placa
Ticket *buscarRegistroInternoPorNumeroPlaca( std::string numeroPlacas, std::list< Ticket * > registros );

// Busca un ticket por folio
TicketPublico *buscarRegistroPublicoPorFolio( unsigned int folio, std::list< TicketPublico * > registros );

// Busca un ticket por numero de placa
TicketPublico *buscarRegistroPublicoPorNumeroPlaca( std::string numeroPlacas, std::list< TicketPublico * > registros );

// Dado un renglón de la consulta de la base de datos, establece los datos del ticket
void establecerRegistroInternoDesdeRenglon( Ticket *ticket, Row *row );
void establecerRegistroPublicoDesdeRenglon( TicketPublico *ticket, Row *row );

#endif
