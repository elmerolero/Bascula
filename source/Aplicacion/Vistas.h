#ifndef VISTAS_H
#define VISTAS_H

#include <gtk/gtk.h>
#include "ContenedorRegistros.h"

void vistaConfiguracion( GtkWidget *widget, gpointer ptr );
void vistaBasculaInterna( GtkWidget *widget, gpointer ptr );
void vistaConsultarPesajesInternos();
void vistaObtenerInforme();
void vistaConsultarUsuario();

void internoLimpiarFormulario();

void regresarInicio();
void regresarUsuarios();

#endif
