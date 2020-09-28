#ifndef GESTOR_REGISTROS_H
#define GESTOR_REGISTROS_H
#include <list>
#include "Database.h"
#include "Usuario.h"
#include "Ticket.h"
#include "ContenedorRegistros.h"

// Base de datos
extern Database database;

// Usuario
extern Usuario usuario;

// Lista de registros de peso del día en curso
extern unsigned int folioActual;
extern std::list< Ticket * > registrosInternosPendientes;
extern std::list< Ticket * > registroConsultados;

// Registros de los productos y empresas
extern ContenedorRegistros productos;
extern ContenedorRegistros empresas;

// Obtiene el contador de los folios
void obtenerFolioActual();

// Obtiene los tickets registrados del día
void obtenerRegistrosInternosPendientes();

// Realiza un registro del ticket
void crearRegistroPendiente( Ticket *ticket );

// Busca un ticket por folio
Ticket *buscarRegistroInternoPorFolio( unsigned int folio, std::list< Ticket * > registros );

// Busca un ticket por numero de placa
Ticket *buscarRegistroInternoPorNumeroPlaca( std::string numeroPlacas, std::list< Ticket * > registros );

// Dado un renglón de la consulta de la base de datos, establece los datos del ticket
void establecerRegistroInternoDesdeRenglon( Ticket *ticket, Row *row );


#endif
