#ifndef GESTOR_REGISTROS_H
#define GESTOR_REGISTROS_H
#include <list>
#include "Database.h"
#include "Usuario.h"
#include "ContenedorRegistros.h"
#include <gtk/gtk.h>

// Base de datos
extern Database database;

// El usuario seleccionado a consultar
extern Usuario *usuarioConsultado;

// Registros de usuarios
extern std::list< Usuario * > registrosUsuarios;

extern std::string empresa_razon_social;
extern std::string empresa_rfc;
extern std::string empresa_imagen;

// Completador para el nombre del conductor
extern GtkListStore *listaNombresConductor;
extern GtkEntryCompletion *completadorNombresConductor;

// Completador 
extern GtkListStore *listaNumerosPlaca;
extern GtkEntryCompletion *completadorNumerosPlaca;

// Nombre de la empresa
extern std::string nombreEmpresa;

extern bool esInicio;

// Obtiene los tickets 
void obtenerUsuariosRegistrados();

// Realiza operaciones con el nombre de la empresa
void registrarNombreEmpresa( std::string nombre );
void cargarNombreEmpresa();

// Limpia la lista de tickets dada
void limpiarUsuariosRegistrados( std::list< Usuario * > &registros );

// Busca un usuario por su nombre de usuario
Usuario *buscarUsuarioPorNombreUsuario( std::string nombreUsuario, std::list< Usuario * > &registros );

// Actualiza si el usuario es o no administrador
void actualizarEstadoAdministrador();

void internoSeleccionarDia();
void internoGenerarInforme();

void usuario_ConsultarRegistros( GtkWidget *widget, gpointer info );
void generarCodigoRecuperacion();
void validarCodigoRecuperacion();

void eliminarUsuario();

// Dado un renglón de la consulta de la base de datos, establece los datos del ticket
/*void establecerRegistroInternoDesdeRenglon( Ticket *ticket, Row *row );
void establecerRegistroPublicoDesdeRenglon( TicketPublico *ticket, Row *row );*/

#endif
