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

// El usuario seleccionado a consultar
extern Usuario *usuarioConsultado;

// Contador de folio de registros internos y públicos
extern unsigned int folioActual;
extern unsigned int folioActualPublico;

// Lista de registros internos
extern std::list< Ticket * > registrosInternosPendientes;
extern std::list< Ticket * > registrosInternosConsultados;

// Lista de registros publicos
extern std::list< TicketPublico * > registrosPublicosPendientes;
extern std::list< TicketPublico * > registrosPublicosConsultados;

// Registros de los productos y empresas
extern ContenedorRegistros productos;
extern ContenedorRegistros empresas;

// Registros de usuarios
extern std::list< Usuario * > registrosUsuarios;

// Primer inicio
extern bool esInicio;

// Nombre de la empresa
extern std::string nombreEmpresa;

// Obtiene el contador de los folios
void obtenerFolioActualInterno();
void obtenerFolioActualPublico();

// Obtiene los tickets 
void obtenerRegistrosPublicosPendientes();
void obtenerRegistrosInternosPendientes();
void obtenerUsuariosRegistrados();

// Obtiene los tickets registrados del día
void obtenerRegistrosPublicosDiarios();
void obtenerRegistrosInternosDiarios();

// Realiza un registro del ticket
void crearRegistroPendiente( Ticket *ticket );

// finaliza un registro
void finalizarRegistro( Ticket *ticket );
void finalizarRegistroPublico( TicketPublico *registroPublico );

// Realiza operaciones con el nombre de la empresa
void registrarNombreEmpresa( std::string nombre );
bool cargarNombreEmpresa();

// Limpia la lista de tickets dada
void limpiarRegistrosInternos( std::list< Ticket * > &registros );
void limpiarRegistrosPublicos( std::list< TicketPublico * > &registros );
void limpiarUsuariosRegistrados( std::list< Usuario * > &registros );

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

// Busca un usuario por su nombre de usuario
Usuario *buscarUsuarioPorNombreUsuario( std::string nombreUsuario, std::list< Usuario * > &registros );

// Actualiza si el usuario es o no administrador
void actualizarEstadoAdministrador();

void internoSeleccionarDia();
void internoGenerarInforme();

void actualizarRegistrosUsuarios( std::list< Usuario * > &usuarios, std::string idContenedor );
void generarCodigoRecuperacion();
void validarCodigoRecuperacion();

void eliminarUsuario();

// Dado un renglón de la consulta de la base de datos, establece los datos del ticket
void establecerRegistroInternoDesdeRenglon( Ticket *ticket, Row *row );
void establecerRegistroPublicoDesdeRenglon( TicketPublico *ticket, Row *row );

#endif
