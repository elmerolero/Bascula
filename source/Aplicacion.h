#ifndef APLICACION_H
#define APLICACION_H
#define APLICACION_H
#include <array>
#include <list>
#include "Widget.h"
#include "Database.h"
#include "Ticket.h"
#include "ContenedorRegistros.h"

// La base de datos
extern Database database;

// Interfaz
extern Widget interfaz;
extern std::string vistaActual;

// Lista de tickets registrados del día en curso
extern unsigned int folioActual;
extern std::list< Ticket * > ticketsPendientes;
extern std::list< Ticket * > ticketsRegistrados;

// Registros
extern ContenedorRegistros productos;
extern ContenedorRegistros empresas;

// Meses para mostrar la fecha en formato humano
const std::array< std::string, 12 > meses{ "enero", "febrero", "marzo", "abril", "mayo", "junio", "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre" };

// Permite mostrar la vista que se desee
void irHacia( GtkWidget *widget, gpointer ptr );

// Obtiene los tickets registrados
void obtenerTicketsRegistrados();

//
void agregarTicketPendiente( Ticket *ticket );

// Busca un ticket por folio
Ticket *buscarTicketPorFolio( unsigned int folio );

// Busca un ticket por numero de placa
Ticket *buscarTicketPorNumeroPlaca( std::string numeroPlacas );

// Inicializa la aplicacion
void iniciar();
        
// Conecta las señales de cada una de las vistas
void conectarSenales();
        
// Muestra la vista indicada
void mostrarVista( std::string idVista );

// Obtiene la hora
std::string obtenerHora();

// Obtiene la fecha
std::string obtenerFecha();

// Actualiza el tiempo
void actualizarTiempo( GtkWidget *widget, gpointer ptr );

#endif

