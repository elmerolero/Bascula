#ifndef VISTAS_H
#define VISTAS_H

#include <gtk/gtk.h>

void vistaBasculaInterna( GtkWidget *widget, gpointer ptr );
void vistaCrearRegistro( GtkWidget *widget, gpointer ptr );
void vistaLeerPesoBruto();
void vistaLeerPesoTara();
void vistaFinalizarRegistro();

#endif
