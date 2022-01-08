#ifndef IMAGEN_H
#define IMAGEN_H

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <Windows.h>
#include <cmath>
#include "Widget.h"

// Tamaño de l
#define TAMANIO_FOTO 256

struct ImagenCairo{
    GdkPixbuf *superficie;
    int ancho;
    int alto;
};

extern std::string nombreTemporal;
extern cairo_surface_t *imagen_temporal;
extern ImagenCairo imagen;       // Imagen original
extern ImagenCairo imagen_escalada;    // Imagen transformada

extern guint senal_presionado;
extern guint senal_movimiento;
extern guint senal_dibujado;

// Acciones
extern Signal senal_imagen_guardar_edicion;
extern Signal senal_imagen_cancelar_edicion;
/*extern guint senal_imagen_guardar;
extern guint senal_imagen_cancelar;
extern guint senal_imagen_descartar;
extern guint senal_imagen_continuar;
extern guint senal_imagen_omitir;*/

extern int imagen_posicion_x;
extern int imagen_posicion_y;
extern int imagen_ultimo_x;
extern int imagen_ultimo_y;

// Manejadores
void leer_mouse( GtkWidget *widget, GdkEventButton evento, gpointer data );
void seleccionar_archivo( GtkWidget *widget, gpointer data );
void cambiar_tamanio( GtkWidget *widget, gpointer data );

// Abre una imagen dada la ruta del archivo
void imagen_abrir( ImagenCairo *imagen, std::string nombreArchivo );

// Dibuja la imagen
gboolean imagen_mostrar( GtkWidget *widget, cairo_t *cr, gpointer data );

// Guarda la imagen editada en una ruta específica
void imagen_guardar( std::string rutaArchivo );

// Mueve la imagen de acuerdo el mouse
gboolean imagen_mover( GtkWidget* widget, GdkEventButton evento, gpointer data );

// Crea una imagen escalada a partir de una imagen y dimensiones dadas
void imagen_escalar( ImagenCairo *imagen, ImagenCairo *imagenDestino, int ancho, int alto );

// Función utilitaria, destruye las dimensiones de una imagen
void imagen_destruir( ImagenCairo *imagen );

// Cancela la edición de imagen
gboolean imagen_cancelar();

void imagen_temporal_guardar( std::string nombre );

GdkPixbuf *imagen_cargar_escalar( std::string archivo, int width, int height );

#endif
