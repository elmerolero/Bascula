#ifndef APLICACION_H
#define APLICACION_H
#define APLICACION_H
#include <array>
#include "Widget.h"
#include "Database.h"
#include "ContenedorRegistros.h"

// Interfaz
extern Widget interfaz;
extern std::string vistaActual;

// Meses para mostrar la fecha en formato humano
const std::array< std::string, 12 > meses{ "enero", "febrero", "marzo", "abril", "mayo", "junio", "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre" };

// Aplicacion
extern bool aplicacionActiva;

extern int numeroMagico;

// Permite mostrar la vista que se desee
void irHacia( GtkWidget *widget, gpointer ptr );

// Inicializa la aplicacion
void iniciar();
       
// Conecta las señales necesarias para funcionar cuando no se ha iniciado sesión
void conectarSenalesBase();

// Conecta las señales de cada una de las vistas
void conectarSenales();
        
// Muestra la vista indicada
void mostrarVista( std::string idVista );

// Abre el cuadro de mensaje con el mensaje indicado
void mostrarMensaje( std::string mensaje );

// Cierra el cuadro de mensaje
void aceptar( GtkWidget *widget, gpointer ptr );

// Obtiene la hora
std::string obtenerHora();

// Obtiene la fecha
std::string obtenerFecha();

// Primer inicio
void primerInicio();

// Actualiza el tiempo
void actualizarTiempo( GtkWidget *widget, gpointer ptr );

#endif

