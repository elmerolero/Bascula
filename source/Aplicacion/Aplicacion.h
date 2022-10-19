#ifndef APLICACION_H
#define APLICACION_H
#define APLICACION_H
#include <array>
#include "Widget.h"
#include "Database.h"
#include "Empresa.h"
#include "ContenedorRegistros.h"

extern time_t t;
extern struct tm tiempo;

// Meses para mostrar la fecha en formato humano
const std::array< std::string, 12 > meses{ "enero", "febrero", "marzo", "abril", "mayo", "junio", "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre" };

// Aplicacion
extern bool aplicacionActiva;

// Permite mostrar la vista que se desee
void irHacia( GtkWidget *widget, gpointer ptr );

// Permite mostrar la vista que se desea
void irA( std::string idVista, bool reiniciarPila );

// Inicializa la aplicacion
void app_iniciar( GtkApplication *aplicacion, gpointer informacion );

void empresa_domicilio_cancelar( GtkWidget *widget, gpointer info );

// Carga la información necesaria para funcionar
void cargarInformacion();

// Conecta las señales necesarias para funcionar cuando no se ha iniciado sesión
void conectarSenalesBase();

// Conecta las señales de cada una de las vistas
void conectarSenales();

// Señales disponibles solo para administrador
void conectarSenalesAdministrador();

// Muestra la vista indicada
void mostrarVista( std::string idVista );

// Abre el cuadro de mensaje con el mensaje indicado
void mostrarMensaje( std::string mensaje );

// Limpiar pila vistas
void reiniciarPilaVistas( void );

void regresarVista( void );

// Muestra una alerta
void app_alerta( GtkWidget *widget, gpointer data );

// Cierra el cuadro de mensaje
void app_mostrar_mensaje( std::string mensaje );
void app_aceptar_mensaje( GtkWidget *widget, gpointer ptr );

void app_mostrar_error( std::string mensajeError );
void app_mostrar_exito( std::string mensajeExito );

// Obtiene la hora
std::string tiempo_leer_hora( bool hora_corta );

// Obtiene la fecha
std::string tiempo_construir_fecha( unsigned int dia, unsigned int mes, unsigned int anio );
std::string tiempo_leer_fecha_corta();
std::string tiempo_leer_fecha_larga();

// Obtiene una una fecha dados el día, el mes y el año
std::string obtenerFecha( unsigned int dia, unsigned int mes, unsigned int anio );

// Primer inicio
void primerInicio();

// Actualiza el tiempo
void actualizarTiempo( GtkWidget *widget, gpointer ptr );


#endif
