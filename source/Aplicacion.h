#ifndef APLICACION_H
#define APLICACION_H
#define APLICACION_H
#include <array>
#include <list>
#include "Widget.h"
#include "Database.h"
#include "Ticket.h"
#include "Registro.h"

// La base de datos
extern Database database;

// Interfaz
extern Widget interfaz;
extern std::string vistaActual;

// Completador a usar
extern GtkEntryCompletion * completador;

// Lista de tickets registrados del día en curso
extern unsigned int folioActual;
extern std::list< Ticket * > ticketsPendientes;
extern std::list< Ticket * > ticketsRegistrados;

// Lista de empresas
extern std::list< Registro * > empresas;
extern GtkListStore *listaEmpresas;
extern unsigned int claveEmpresaActual;

// Lista de productos
extern std::list< Registro * > productos;
extern GtkListStore *listaProductos;
extern unsigned int claveProductoActual;

// Meses para mostrar la fecha en formato humano
const std::array< std::string, 12 > meses{ "enero", "febrero", "marzo", "abril", "mayo", "junio", "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre" };

// Permite mostrar la vista que se desee
void irHacia( GtkWidget *widget, gpointer ptr );

// Obtiene los tickets registrados
void obtenerTicketsRegistrados();

// Obtiene las empresas registradas de la base de datos
void obtenerEmpresasRegistradas();

// Obtiene los productos registrados de la base de datos
void obtenerProductosRegistrados();

// Busca un producto con el texto indicado
Registro *buscarRegistro( unsigned int clave, std::list< Registro * > &registros );
Registro *buscarRegistro( std::string nombre, std::list< Registro * > &registros );

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

